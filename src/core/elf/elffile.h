#ifndef ELFFILE_H
#define ELFFILE_H

#include <string>
#include <filesystem>

#include <mtccore/programheader.h>
#include <mtccore/sectionheader.h>

namespace MTC {

    class MTC_CORE_EXPORT ElfFile {
    public:
        ElfFile();
        ~ElfFile();

        enum Type {
            Executable,
            Dynamic,
        };

        enum Architecture {
            AMD64,
            AArch64,
            RiscV64,
        };

    public:
        bool load(const std::filesystem::path &path) const;

        std::filesystem::path filePath() const;
        std::string errorMessage() const;

        Type type() const;
        Architecture architecture() const;

        int programHeaderCount() const;
        ProgramHeader programHeader(int index) const;

        int sectionHeaderCount() const;
        SectionHeader sectionHeader(int index) const;

    protected:
        class Impl;
        std::unique_ptr<Impl> _impl;
    };

}

#endif // ELFFILE_H
