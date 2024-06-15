#include "ArenaString.h"

namespace se::string
{
    ArenaString vArenaFormat(const std::string_view& fmt, const std::format_args& args, memory::ArenaAllocator<char>& alloc)
    {
        ArenaString str(alloc);
        std::vformat_to(std::back_insert_iterator{str}, fmt, args);
        return str;
    }
}
