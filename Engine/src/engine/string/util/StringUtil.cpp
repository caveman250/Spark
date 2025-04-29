#include "StringUtil.h"

namespace se::string::util
{
    bool Split(const se::String &string,
                                       se::String &lhs,
                                       se::String &rhs,
                                       char deliminator,
                                       bool fromEnd)
    {
        size_t i = string.Find(deliminator, fromEnd);
        if (i != String::InvalidPos)
        {
            lhs = string.SubString(0, i);
            rhs = string.SubString(i + 1, string.Size());
            return true;
        }

        return false;
    }
}
