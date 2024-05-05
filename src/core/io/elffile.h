#ifndef ELFFILE_H
#define ELFFILE_H

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

#include <mtccore/mtccoreglobal.h>

namespace MTC {

    class MTC_CORE_EXPORT ElfFile {
    public:
        ElfFile();
        ~ElfFile();

        enum FileType {
            Executable,
            Dynamic,
        };

        enum FileArch {
            AMD64,
            AArch64,
            RiscV64,
        };

        class Info;
        class Impl;

        class MTC_CORE_EXPORT ProgramHeader {
        public:
            ProgramHeader() : _index(0) {
            }

        protected:
            std::shared_ptr<Info> _info;
            int _index;

            friend class ElfFile;
        };

        class MTC_CORE_EXPORT SectionHeader {
        public:
            SectionHeader() : _index(0) {
            }

            enum Type {
                Null,
                ProgramBits,
                SymbolTable,
                DynamicSymbol,
                StringTable,
                RelocationWithAttends,
                Hash,
                DynamicLinking,
                Note,
                NoBits,
                Relocation,
                SharedLibrary,
                LowProcessorSpecific,
                HighProcessorSpecific,
                LowUserSpecific,
                HighUserSpecific,
            };

        protected:
            std::shared_ptr<Info> _info;
            int _index;

            friend class ElfFile;
        };

    public:
        bool load(const std::filesystem::path &path) const;

        std::filesystem::path filePath() const;
        std::string errorMessage() const;

        FileType fileType() const;
        FileArch fileArch() const;

        int programHeaderCount() const;
        ProgramHeader programHeader(int index) const;

        int sectionHeaderCount() const;
        SectionHeader sectionHeader(int index) const;

    protected:
        std::unique_ptr<Impl> _impl;
    };

}

#endif // ELFFILE_H
