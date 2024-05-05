#include "sectionheader.h"

#include "elffile_p.h"
#include "format.h"

namespace MTC {

    std::string SectionHeader::name() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).name;
    }

    SectionHeader::Type SectionHeader::type() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).type;
    }

    size_t SectionHeader::osType() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).osType;
    }

    int SectionHeader::attributes() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).attr;
    }

    uintptr_t SectionHeader::address() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).address;
    }

    const char *SectionHeader::data() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).data.data();
    }

    size_t SectionHeader::dataSize() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).data.size();
    }

    uint32_t SectionHeader::link() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).link;
    }

    uint32_t SectionHeader::info() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).info;
    }

    size_t SectionHeader::addressAlign() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).addressAlign;
    }

    size_t SectionHeader::entrySize() const {
        if (!_container) {
            return {};
        }
        return _container->sectionHeaders.at(_index).entrySize;
    }

    std::vector<std::string> SectionHeader::asStringTable() const {
        if (!_container) {
            return {};
        }
        const auto &data = _container->sectionHeaders.at(_index).data;
        return extractNullSeperatedStrings(data.data(), data.size());
    }

}