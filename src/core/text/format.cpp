#include "format.h"

namespace MTC {

    std::string formatText(const std::string &format, const std::vector<std::string> &args) {
        std::string result = format;
        for (size_t i = 0; i < args.size(); i++) {
            std::string placeholder = "%" + std::to_string(i + 1);
            size_t pos = result.find(placeholder);
            while (pos != std::string::npos) {
                result.replace(pos, placeholder.length(), args[i]);
                pos = result.find(placeholder, pos + args[i].size());
            }
        }
        return result;
    }

    std::vector<std::string> extractNullSeperatedStrings(const char *data, size_t size) {
        std::vector<std::string> res;
        std::string part;
        for (size_t i = 0; i < size; ++i) {
            const auto &ch = data[i];
            if (ch == '\0') {
                res.push_back(part);
                part.clear();
                continue;
            }
            part += ch;
        }
        return res;
    }

}