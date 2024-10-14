#include "String.h"

namespace se
{
    String::String()
    {
    }

    String::String(const char* str)
    {
        auto len = strlen(str);
        m_Data.reserve(len);
        for (size_t i = 0; i < len; ++i)
        {
            m_Data.push_back(str[i]);
        }
    }

    String::String(const String& rhs)
    {
        m_Data = rhs.m_Data;
    }

    String::String(const std::string& rhs)
    {
        auto len = rhs.size();
        m_Data.reserve(len);
        for (size_t i = 0; i < len; ++i)
        {
            m_Data.push_back(rhs[i]);
        }
    }

    bool String::operator==(const String& rhs) const
    {
        if (Size() != rhs.Size())
        {
            return false;
        }

        for (size_t i = 0; i < Size(); ++i)
        {
            if (m_Data[i] != rhs[i])
            {
                return false;
            }
        }

        return true;
    }

    void String::operator=(const String& rhs)
    {
        m_Data = rhs.m_Data;
    }

    void String::operator=(const char* rhs)
    {
        *this = String(rhs);
    }

    char String::operator[](size_t i) const
    {
        return m_Data[i];
    }
}
