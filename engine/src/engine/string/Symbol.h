#pragma once

#include <spark.h>
#include "SymbolRegistry.h"

namespace se::string
{
    class Symbol
    {
    public:
        constexpr Symbol() noexcept : m_ID(0) {}

        Symbol(const char* str)
            : m_ID(SymbolRegistry::Get().FindOrAdd(str)) {}

        Symbol(std::string_view str)
            : m_ID(SymbolRegistry::Get().FindOrAdd(str)) {}

        explicit Symbol(const std::string& str)
            : m_ID(SymbolRegistry::Get().FindOrAdd(str)) {}

        explicit constexpr Symbol(uint32_t id) noexcept : m_ID(id) {}

        constexpr bool operator==(const Symbol& other) const noexcept
        {
            return m_ID == other.m_ID;
        }

        constexpr bool operator!=(const Symbol& other) const noexcept
        {
            return m_ID != other.m_ID;
        }

        constexpr bool operator<(const Symbol& other) const noexcept
        {
            return m_ID < other.m_ID;
        }

        constexpr uint32_t GetID() const noexcept { return m_ID; }
        constexpr bool IsNone() const noexcept;

        std::string_view ToStringView() const;
        std::string ToString() const;
        const char* c_str() const;

    private:
        uint32_t m_ID;
    };
}

namespace std
{
    template <>
    struct hash<se::string::Symbol>
    {
        std::size_t operator()(const se::string::Symbol& name) const noexcept
        {
            return std::hash<uint32_t>{}(name.GetID());
        }
    };

    template <> struct formatter<se::string::Symbol>
    {
        static constexpr auto parse(const std::format_parse_context& ctx)
        {
            return ctx.begin();
        }

        template <typename FormatContext>
        auto format(const se::string::Symbol& obj, FormatContext& ctx) const
        {
            return std::format_to(ctx.out(), "{}", obj.ToStringView());
        }
    };
}