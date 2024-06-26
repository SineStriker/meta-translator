#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <typeinfo>
#include <vector>
#include <filesystem>
#include <sstream>

#include <mtccore/mtccoreglobal.h>

namespace MTC {

    template <class T>
    std::string anyToString(T &&t) {
        using T2 = std::remove_cv_t<std::remove_reference_t<T>>;
        if constexpr (std::is_same_v<T2, bool>) {
            return t ? "true" : "false";
        } else if constexpr (std::is_integral_v<T2>) {
            return std::to_string(t);
        } else if constexpr (std::is_floating_point_v<T2>) {
            std::ostringstream oss;
            oss << std::noshowpoint << t;
            return oss.str();
        } else if constexpr (std::is_same_v<T2, std::filesystem::path>) {
            return t.string();
        } else {
            return std::string(t);
        }
    }

    std::string formatText(const std::string &format, const std::vector<std::string> &args);

    template <typename... Args>
    auto formatTextN(const std::string &format, Args &&...args) {
        return formatText(format, {anyToString(std::forward<decltype(args)>(args))...});
    }

    std::vector<std::string> extractNullSeperatedStrings(const char *data, size_t size);

}

#endif // FORMAT_H
