#pragma once
#include "engine/memory/ArenaAllocator.h"

namespace se::string
{
    typedef std::basic_string<char, std::char_traits<char>, memory::ArenaAllocator<char> > ArenaString;
    typedef std::basic_stringstream<char, std::char_traits<char>, memory::ArenaAllocator<char> > ArenaStringStream;

    ArenaString vArenaFormat(const std::string_view& fmt, const std::format_args& args, memory::ArenaAllocator<char>& alloc);

    template <class... Types>
    ArenaString ArenaFormat(const std::format_string<Types...> fmt, memory::ArenaAllocator<char>& alloc, Types&&... args)
    {
        return vArenaFormat(fmt.get(), std::make_format_args(args...), alloc);
    }
}
