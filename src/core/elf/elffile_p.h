#ifndef ELFFILE_P_H
#define ELFFILE_P_H

#include <vector>

#include <mtccore/elffile.h>
#include <mtccore/programheader.h>
#include <mtccore/sectionheader.h>

namespace MTC {

    class ProgramHeaderData {
    public:
        ProgramHeader::Type type{};
        size_t osType{};
        int attr{};
        uintptr_t physicalAddress{};
        uintptr_t virtualAddress{};
        size_t memSize{};
        size_t align{};
        std::vector<char> data;
    };

    class SectionHeaderData {
    public:
        std::string name;
        SectionHeader::Type type{};
        size_t osType{};
        int attr{};
        uintptr_t address{};
        size_t addressAlign{};
        std::vector<char> data;

        uint32_t link{};
        uint32_t info{};
        uint64_t entrySize{};
    };

    class ElfFileSharedContainer {
    public:
        std::filesystem::path path;

        ElfFile::Type type{};
        ElfFile::Architecture arch{};

        std::vector<ProgramHeaderData> programHeaders;
        std::vector<SectionHeaderData> sectionHeaders;

        std::vector<std::string> sectionHeaderStringTable;
    };

    class ElfFile::Impl {
    public:
        std::shared_ptr<ElfFileSharedContainer> container;
        std::string err;
    };


}

#endif // ELFFILE_P_H
