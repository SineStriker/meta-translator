#include "elffile.h"
#include "elffile_p.h"

#include <fstream>
#include <cstring>
#include <sstream>

#include "elf.h"
#include "stream.h"
#include "format.h"

namespace fs = std::filesystem;

namespace MTC {

    ElfFile::ElfFile() : _impl(std::make_unique<Impl>()) {
    }

    ElfFile::~ElfFile() {
    }

    bool ElfFile::load(const fs::path &path) const {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            return false;

        ElfFileSharedContainer container;

        // Read header
        ::Elf64_Ehdr header;
        file.read(reinterpret_cast<char *>(&header), sizeof(header));
        if (!file.good()) {
            _impl->err = formatTextN("%1: Failed to read ELF header", path);
            return false;
        }

        // Check header
        if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0) {
            _impl->err = formatTextN("%1: Not an ELF file, sign not match\n", path);
            return false;
        }
        if (header.e_ident[EI_CLASS] != ELFCLASS64) {
            _impl->err = formatTextN("%1: Not an 64-bit ELF file\n", path);
            return false;
        }
        if (header.e_ident[EI_DATA] != ELFDATA2LSB) {
            _impl->err = formatTextN("%1: Not a little-endian ELF file\n", path);
            return false;
        }
        if (header.e_ident[EI_VERSION] != EV_CURRENT) {
            _impl->err =
                formatTextN("%1: Unknown ELF version (%2)\n", path, header.e_ident[EI_VERSION]);
            return false;
        }
        if (header.e_ident[EI_OSABI] != ELFOSABI_LINUX &&
            header.e_ident[EI_OSABI] != ELFOSABI_SYSV) {
            _impl->err = formatTextN("%1: Not a Linux ELF file\n", path);
            return false;
        }
        switch (header.e_type) {
            case ET_EXEC:
                container.type = Executable;
                break;
            case ET_DYN:
                container.type = Dynamic;
                break;
            default:
                _impl->err = formatTextN("%1: Unknown file type (%2)\n", path, header.e_type);
                return false;
        }
        switch (header.e_machine) {
            case EM_X86_64:
                container.arch = AMD64;
                break;
            case EM_AARCH64:
                container.arch = AArch64;
                break;
            case EM_RISCV:
                container.arch = RiscV64;
                break;
            default:
                _impl->err =
                    formatTextN("%1: Unknown file architecture (%2)\n", path, header.e_machine);
                return false;
        }
        if (header.e_phentsize != sizeof(Elf64_Phdr)) {
            _impl->err = formatTextN("%1: Program Header Entry size mismatch (%2 != %3)\n", path,
                                     header.e_phentsize, sizeof(Elf64_Phdr));
            return false;
        }
        if (header.e_shentsize != sizeof(Elf64_Shdr) && header.e_shentsize != 0) {
            _impl->err = formatTextN("%1: Section Header Entry size mismatch (%2 != %3)\n", path,
                                     header.e_shentsize, sizeof(Elf64_Shdr));
            return false;
        }

        // Read section headers
        if (header.e_shentsize != 0) {
            auto readSection = [&](::Elf64_Shdr &section) {
                file.read(reinterpret_cast<char *>(&section), sizeof(section));
                if (!file.good()) {
                    _impl->err = formatTextN("%1: Failed to read section header", path);
                    return false;
                }
                return true;
            };

            auto readData = [&](const ::Elf64_Shdr &section, SectionHeaderData &sh) {
                if (section.sh_size == 0) {
                    return true;
                }
                auto orgOffset = file.tellg();
                file.seekg(section.sh_offset);

                sh.data.resize(section.sh_size);
                file.read(sh.data.data(), section.sh_size);
                file.seekg(orgOffset);
                if (!file.good()) {
                    _impl->err = formatTextN("%1: Failed to read section data", path);
                    return false;
                }
                return true;
            };

            // The number of entries in the section header table. The product of e_shentsize and
            // e_shnum gives the section header table's size in bytes. If a file has no section
            // header table, e_shnum holds the value zero.
            size_t sectionCount = header.e_shnum;
            if (sectionCount == 0) {
                file.seekg(header.e_shoff);
                ::Elf64_Shdr section;
                if (!readSection(section)) {
                    return false;
                }
                sectionCount = section.sh_size;
            }

            std::vector<size_t> nameIndexes;
            nameIndexes.reserve(sectionCount);
            container.sectionHeaders.reserve(sectionCount);

            file.seekg(header.e_shoff);
            for (int i = 0; i < sectionCount; ++i) {
                ::Elf64_Shdr section;
                if (!readSection(section)) {
                    return false;
                }

                nameIndexes.push_back(section.sh_name);

                SectionHeaderData sh;
                SectionHeader::Type type = SectionHeader::OSSpecific;
                switch (section.sh_type) {
                    case SHT_NULL:
                        type = SectionHeader::Null;
                        break;
                    case SHT_PROGBITS:
                        type = SectionHeader::ProgramBits;
                        break;
                    case SHT_SYMTAB:
                        type = SectionHeader::SymbolTable;
                        break;
                    case SHT_DYNSYM:
                        type = SectionHeader::DynamicSymbol;
                        break;
                    case SHT_STRTAB:
                        type = SectionHeader::StringTable;
                        break;
                    case SHT_RELA:
                        type = SectionHeader::RelocationWithAttends;
                        break;
                    case SHT_HASH:
                        type = SectionHeader::Hash;
                        break;
                    case SHT_DYNAMIC:
                        type = SectionHeader::DynamicLinking;
                        break;
                    case SHT_NOTE:
                        type = SectionHeader::Note;
                        break;
                    case SHT_NOBITS:
                        type = SectionHeader::NoBits;
                        break;
                    case SHT_REL:
                        type = SectionHeader::Relocation;
                        break;
                    case SHT_SHLIB:
                        type = SectionHeader::SharedLibrary;
                        break;
                    case SHT_LOPROC:
                        type = SectionHeader::LowProcessorSpecific;
                        break;
                    case SHT_HIPROC:
                        type = SectionHeader::HighProcessorSpecific;
                        break;
                    case SHT_LOUSER:
                        type = SectionHeader::LowUserSpecific;
                        break;
                    case SHT_HIUSER:
                        type = SectionHeader::HighUserSpecific;
                        break;
                    default:
                        break;
                }
                sh.type = type;
                sh.osType = section.sh_type;

                int attr = 0;
                if (section.sh_flags & SHF_WRITE) {
                    attr |= SectionHeader::Writable;
                }
                if (section.sh_flags & SHF_ALLOC) {
                    attr |= SectionHeader::AllocationRequired;
                }
                if (section.sh_flags & SHF_EXECINSTR) {
                    attr |= SectionHeader::Executable;
                }
                sh.attr = attr;

                sh.address = section.sh_addr;
                sh.link = section.sh_link;
                sh.info = section.sh_info;
                sh.addressAlign = section.sh_addralign;
                sh.entrySize = section.sh_entsize;

                if (sh.type != SectionHeader::NoBits) {
                    if (!readData(section, sh)) {
                        return false;
                    }
                }

                container.sectionHeaders.emplace_back(sh);
            }

            // Read section header names
            if (header.e_shstrndx >= container.sectionHeaders.size() ||
                container.sectionHeaders.at(header.e_shstrndx).type != SectionHeader::StringTable) {
                _impl->err =
                    formatTextN("%1: Invalid section header index (%2)", path, header.e_shstrndx);
                return false;
            } else {
                const auto &data = container.sectionHeaders.at(header.e_shstrndx).data;
                for (size_t i = 0; i < container.sectionHeaders.size(); ++i) {
                    if (nameIndexes[i] >= data.size()) {
                        _impl->err = formatTextN("%1: Invalid name index of section %2 (%3)", path,
                                                 i, nameIndexes[i]);
                        return false;
                    }
                    container.sectionHeaders[i].name = data.data() + nameIndexes[i];
                }
            }
        }

        // Read program headers
        {
            auto readSection = [&](::Elf64_Phdr &section) {
                file.read(reinterpret_cast<char *>(&section), sizeof(section));
                if (!file.good()) {
                    _impl->err = formatTextN("%1: Failed to read program header", path);
                    return false;
                }
                return true;
            };

            auto readData = [&](const ::Elf64_Phdr &section, ProgramHeaderData &ph) {
                if (section.p_filesz == 0) {
                    return true;
                }
                auto orgOffset = file.tellg();
                file.seekg(section.p_offset);

                ph.data.resize(section.p_filesz);
                file.read(ph.data.data(), section.p_filesz);
                file.seekg(orgOffset);
                if (!file.good()) {
                    _impl->err = formatTextN("%1: Failed to read program data", path);
                    return false;
                }
                return true;
            };

            container.programHeaders.reserve(header.e_phnum);

            file.seekg(header.e_phoff);
            for (int i = 0; i < header.e_phnum; ++i) {
                ::Elf64_Phdr section;
                if (!readSection(section)) {
                    return false;
                }

                ProgramHeaderData ph;

                ProgramHeader::Type type = ProgramHeader::OSSpecific;
                switch (section.p_type) {
                    case PT_NULL:
                        type = ProgramHeader::Null;
                        break;
                    case PT_LOAD:
                        type = ProgramHeader::Loadable;
                        break;
                    case PT_DYNAMIC:
                        type = ProgramHeader::DynamicLinking;
                        break;
                    case PT_INTERP:
                        type = ProgramHeader::Interpreter;
                        break;
                    case PT_NOTE:
                        type = ProgramHeader::Note;
                        break;
                    case PT_SHLIB:
                        type = ProgramHeader::SharedLibrary;
                        break;
                    case PT_PHDR:
                        type = ProgramHeader::ProgramHeaderInfo;
                        break;
                    case PT_LOOS:
                        type = ProgramHeader::LowOSSpecific;
                        break;
                    case PT_HIOS:
                        type = ProgramHeader::HighOSSpecific;
                        break;
                    case PT_LOPROC:
                        type = ProgramHeader::LowProcessorSpecific;
                        break;
                    case PT_HIPROC:
                        type = ProgramHeader::HighProcessorSpecific;
                        break;
                    default:
                        break;
                }
                ph.type = type;
                ph.osType = section.p_type;

                int attr = 0;
                if (section.p_flags & PF_X) {
                    attr |= ProgramHeader::Executable;
                }
                if (section.p_flags & PF_W) {
                    attr |= ProgramHeader::Writable;
                }
                if (section.p_flags & PF_R) {
                    attr |= ProgramHeader::Readable;
                }
                ph.attr = attr;
                ph.physicalAddress = section.p_paddr;
                ph.virtualAddress = section.p_vaddr;

                ph.memSize = section.p_memsz;
                ph.align = section.p_align;

                if (!readData(section, ph)) {
                    return false;
                }

                container.programHeaders.emplace_back(ph);
            }
        }

        container.path = path;
        _impl->container = std::make_shared<decltype(container)>(std::move(container));
        return true;
    }

    fs::path ElfFile::filePath() const {
        if (!_impl->container)
            return {};
        return _impl->container->path;
    }

    std::string ElfFile::errorMessage() const {
        return _impl->err;
    }

    ElfFile::Type ElfFile::type() const {
        if (!_impl->container)
            return {};
        return _impl->container->type;
    }

    ElfFile::Architecture ElfFile::architecture() const {
        if (!_impl->container)
            return {};
        return _impl->container->arch;
    }

    int ElfFile::programHeaderCount() const {
        if (!_impl->container)
            return {};
        return int(_impl->container->programHeaders.size());
    }

    ProgramHeader ElfFile::programHeader(int index) const {
        if (!_impl->container)
            return {};

        ProgramHeader res;
        res._container = _impl->container;
        res._index = index;
        return res;
    }

    int ElfFile::sectionHeaderCount() const {
        if (!_impl->container)
            return {};
        return int(_impl->container->sectionHeaders.size());
    }

    SectionHeader ElfFile::sectionHeader(int index) const {
        if (!_impl->container)
            return {};

        SectionHeader res;
        res._container = _impl->container;
        res._index = index;
        return res;
    }

}