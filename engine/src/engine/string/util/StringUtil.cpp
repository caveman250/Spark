#include "StringUtil.h"
#include "engine/string/String.h"

namespace se::string::util
{
    bool Split(const String &string,
                                       String &lhs,
                                       String &rhs,
                                       char deliminator,
                                       bool fromEnd)
    {
        size_t i = string.Find(deliminator, fromEnd);
        if (i != String::InvalidPos)
        {
            lhs = string.SubString(0, i);
            if (i < string.Size() - 1)
            {
                rhs = string.SubString(i + 1, string.Size());
            }
            else
            {
                rhs = {};
            }
            return true;
        }

        return false;
    }
}
