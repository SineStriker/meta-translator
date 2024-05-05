#include "elffile.h"

#include <fstream>
#include <cstring>

#include "elf.h"
#include "stream.h"
#include "format.h"

namespace fs = std::filesystem;

namespace MTC {

    struct ElfFile::Info {
        FileType fileType{};
        FileArch fileArch{};

        class PH {
        public:
        };
        std::vector<PH> phList;

        class SH {
        public:
        };
        std::vector<SH> shList;
    };

    class ElfFile::Impl {
    public:
        fs::path path;
        std::shared_ptr<Info> info;
        std::string err;
    };

    ElfFile::ElfFile() : _impl(std::make_unique<Impl>()) {
    }

    ElfFile::~ElfFile() {
    }

    bool ElfFile::load(const fs::path &path) const {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        Info info;

        Elf64_Ehdr header;

        // Read header
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
            header.e_ident[EI_OSABI] != ELFOSABI_NONE &&
            header.e_ident[EI_OSABI] != ELFOSABI_SYSV) {
            _impl->err = formatTextN("%1: Not a Linux ELF file\n", path);
            return false;
        }
        switch (header.e_type) {
            case ET_EXEC:
                info.fileType = Executable;
                break;
            case ET_DYN:
                info.fileType = Dynamic;
                break;
            default:
                _impl->err = formatTextN("%1: Unknown file type (%2)\n", path, header.e_type);
                return false;
        }
        switch (header.e_machine) {
            case EM_X86_64:
                info.fileArch = AMD64;
                break;
            case EM_AARCH64:
                info.fileArch = AArch64;
                break;
            case EM_RISCV:
                info.fileArch = RiscV64;
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

        // Read sections
        if(header.e_shentsize && header.e_shnum) {

        }

        _impl->path = path;
        _impl->info = std::make_shared<Info>(std::move(info));
        return true;
    }

    fs::path ElfFile::filePath() const {
        return _impl->path;
    }

    std::string ElfFile::errorMessage() const {
        return _impl->err;
    }


    ElfFile::FileType ElfFile::fileType() const {
        auto info = _impl->info.get();
        if (!info) {
            return {};
        }
        return info->fileType;
    }

    ElfFile::FileArch ElfFile::fileArch() const {
        auto info = _impl->info.get();
        if (!info) {
            return {};
        }
        return info->fileArch;
    }

    int ElfFile::programHeaderCount() const {
        auto info = _impl->info.get();
        if (!info) {
            return {};
        }
        return int(info->phList.size());
    }

    ElfFile::ProgramHeader ElfFile::programHeader(int index) const {
        auto info = _impl->info.get();
        if (!info) {
            return {};
        }

        ProgramHeader res;
        res._info = _impl->info;
        res._index = index;
        return res;
    }

    int ElfFile::sectionHeaderCount() const {
        auto info = _impl->info.get();
        if (!info) {
            return {};
        }
        return int(info->shList.size());
    }

    ElfFile::SectionHeader ElfFile::sectionHeader(int index) const {
        SectionHeader res;
        res._info = _impl->info;
        res._index = index;
        return res;
    }

}