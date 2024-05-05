#include "programheader.h"

#include "elffile_p.h"

namespace MTC {

    ProgramHeader::Type ProgramHeader::type() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).type;
    }

    size_t ProgramHeader::osType() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).osType;
    }

    int ProgramHeader::attributes() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).attr;
    }

    uintptr_t ProgramHeader::physicalAddress() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).physicalAddress;
    }

    uintptr_t ProgramHeader::virtualAddress() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).virtualAddress;
    }

    size_t ProgramHeader::memorySize() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).memSize;
    }

    size_t ProgramHeader::align() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).align;
    }

    const char *ProgramHeader::data() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).data.data();
    }

    size_t ProgramHeader::dataSize() const {
        if (!_container) {
            return {};
        }
        return _container->programHeaders.at(_index).data.size();
    }

}