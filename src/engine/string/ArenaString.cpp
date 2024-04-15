#include "ArenaString.h"

namespace se::string
{
    ArenaString vArenaFormat(const std::string_view _Fmt, const std::format_args _Args, memory::ArenaAllocator<char>& alloc)
    {
        ArenaString _Str(alloc);
        _Str.reserve(_Fmt.size() + _Args._Estimate_required_capacity());
        _STD vformat_to(std::back_insert_iterator{_Str}, _Fmt, _Args);
        return _Str;
    }
}
