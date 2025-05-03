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
        String& operator+=(char c);
        String& operator+=(const char* c);
        String& operator+=(const String& str);
        String& operator--();

        void Insert(size_t i, char c);
        void Insert(size_t i, const char* str);
        void Insert(size_t i, const String& str);
        void Erase(size_t i);
        String& Replace(size_t start, size_t end, const String& str);

        bool Contains(const String& str) const;
        bool StartsWith(const String& str) const;
        bool EndsWith(const String& str) const;
        size_t Find(char c, bool fromEnd) const;
        static size_t InvalidPos;

        String SubString(size_t begin, size_t end) const;

        const char* Data() const { return m_Data.data(); }
        size_t Size() const { return m_Data.size() - 1; }

    private:
        std::vector<char> m_Data = {};
    };

    String operator+(const String& lhs, const String &rhs);
    String operator+(const String& lhs, char rhs);
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

template <>
struct std::hash<se::String>
{
    std::size_t operator()(const se::String& rhs) const
    {
        return std::hash<std::string>()(rhs.Data());
    }
};

std::ostream& operator<<(std::ostream& os, const se::String& string);
