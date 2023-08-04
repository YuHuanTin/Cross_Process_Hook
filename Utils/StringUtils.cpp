
#include "StringUtils.h"

namespace Utils {
    std::string stringToLower(const std::string &RawString) {
        std::string newString = RawString;
        for (auto &one: newString) {
            if (isalpha(one) && isupper(one)) {
                one = tolower(one);
            }
        }
        return newString;
    }

}
