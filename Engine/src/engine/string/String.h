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
        void operator=(const String& rhs);
        void operator=(const char* rhs);
        char operator[](size_t i) const;

        const char* Data() const { return m_Data.data(); }
        size_t Size() const { return m_Data.size(); }

    private:
        std::vector<char> m_Data = {};
    };
}
