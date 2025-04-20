#pragma once

#include "spark.h"

namespace se
{
    class String
    {
    public:
        String();

        String(const char* str);
        String(const String& rhs);
        String(const std::string& rhs);

        bool operator==(const String& rhs) const;
        String& operator=(const String& rhs);
        String& operator=(const char* rhs);
        char operator[](size_t i) const;

        const char* Data() const { return m_Data.data(); }
        size_t Size() const { return m_Data.size() - 1; }

    private:
        std::vector<char> m_Data = {};
    };
}

template <>
struct std::formatter<se::String>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <class FormatContext>
    auto format(const se::String& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", obj.Data());
    }
};
