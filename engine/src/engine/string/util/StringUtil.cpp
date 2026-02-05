#include "StringUtil.h"

namespace se::string::util
{
    bool Split(const std::string &string,
                                       std::string &lhs,
                                       std::string &rhs,
                                       char deliminator,
                                       bool fromEnd)
    {
        size_t i = fromEnd ? string.rfind(deliminator) : string.find(deliminator);
        if (i != std::string::npos)
        {
            lhs = string.substr(0, i);
            if (i < string.size() - 1)
            {
                rhs = string.substr(i + 1, string.size());
            }
            else
            {
                rhs = {};
            }
            return true;
        }

        return false;
    }

    std::string ToLower(std::string s)
    {
        std::ranges::transform(s, s.begin(), [](int c){ return static_cast<char>(std::tolower(c)); });
        return s;
    }
}
