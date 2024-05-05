#ifndef PROGRAMHEADER_H
#define PROGRAMHEADER_H

#include <string>
#include <memory>

#include <mtccore/mtccoreglobal.h>

namespace MTC {

    class ElfFile;

    class ElfFileSharedContainer;

    class MTC_CORE_EXPORT ProgramHeader {
    public:
        ProgramHeader() : _index(0) {
        }
        ~ProgramHeader() = default;

        enum Type {
            Null,
            Loadable,
            DynamicLinking,
            Interpreter,
            Note,
            SharedLibrary,
            ProgramHeaderInfo,
            LowOSSpecific,
            HighOSSpecific,
            LowProcessorSpecific,
            HighProcessorSpecific,
            OSSpecific,
        };

        enum Attribute {
            Executable,
            Writable,
            Readable,
        };

    public:
        Type type() const;
        size_t osType() const;
        int attributes() const;
        uintptr_t physicalAddress() const;
        uintptr_t virtualAddress() const;

        size_t memorySize() const;
        size_t align() const;

        const char *data() const;
        size_t dataSize() const;

    protected:
        std::shared_ptr<ElfFileSharedContainer> _container;
        size_t _index;

        friend class ElfFile;
    };

}

#endif // PROGRAMHEADER_H
