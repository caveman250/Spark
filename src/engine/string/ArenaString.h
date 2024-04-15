#pragma once
#include "engine/memory/ArenaAllocator.h"

namespace se::string
{
    typedef std::basic_string<char, std::char_traits<char>, memory::ArenaAllocator<char> > ArenaString;
    typedef std::basic_stringstream<char, std::char_traits<char>, memory::ArenaAllocator<char> > ArenaStringStream;

    ArenaString vArenaFormat(const std::string_view _Fmt, const std::format_args _Args, memory::ArenaAllocator<char>& alloc);

    template <class... _Types>
    ArenaString ArenaFormat(const std::format_string<_Types...> _Fmt, memory::ArenaAllocator<char>& alloc, _Types&&... _Args)
    {
        return vArenaFormat(_Fmt.get(), std::make_format_args(_Args...), alloc);
    }
}
