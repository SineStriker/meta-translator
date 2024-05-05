#ifndef SECTIONHEADER_H
#define SECTIONHEADER_H

#include <string>
#include <memory>
#include <vector>

#include <mtccore/mtccoreglobal.h>

namespace MTC {

    class ElfFile;

    class ElfFileSharedContainer;

    class MTC_CORE_EXPORT SectionHeader {
    public:
        SectionHeader() : _index(0) {
        }
        ~SectionHeader() = default;

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
            OSSpecific,
        };

        enum Attribute {
            Writable,
            AllocationRequired,
            Executable,
        };

    public:
        std::string name() const;
        Type type() const;
        size_t osType() const;
        int attributes() const;
        uintptr_t address() const;

        const char *data() const;
        size_t dataSize() const;

        uint32_t link() const;
        uint32_t info() const;
        size_t addressAlign() const;
        size_t entrySize() const;

        std::vector<std::string> asStringTable() const;

    protected:
        std::shared_ptr<ElfFileSharedContainer> _container;
        size_t _index;

        friend class ElfFile;
    };

}

#endif // SECTIONHEADER_H
