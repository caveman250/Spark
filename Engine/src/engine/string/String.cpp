#include "String.h"

#include "engine/container_util/RangeUtil.h"

namespace se
{
    size_t String::InvalidPos = std::numeric_limits<size_t>::max();

    String::String()
    {
    }

    String::String(const char* str)
    {
        if (str != nullptr)
        {
            auto len = strlen(str);
            m_Data.reserve(len);
            for (size_t i = 0; i < len; ++i)
            {
                m_Data.push_back(str[i]);
            }
        }

        if (m_Data.empty() || m_Data.back() != '\0')
        {
            m_Data.push_back('\0');
        }
    }

    String::String(const String& rhs)
    {
        m_Data = rhs.m_Data;
        if (m_Data.empty() || m_Data.back() != '\0')
        {
            m_Data.push_back('\0');
        }
    }

    String::String(const std::string& rhs)
    {
        auto len = rhs.size();
        m_Data.resize(len);
        std::memcpy(m_Data.data(), rhs.data(), len);
        if (m_Data.back() != '\0')
        {
            m_Data.push_back('\0');
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

    String& String::operator=(const String& rhs)
    {
        m_Data = rhs.m_Data;
        return *this;
    }

    String& String::operator=(const char* rhs)
    {
        *this = String(rhs);
        return *this;
    }

    char String::operator[](size_t i) const
    {
        return m_Data[i];
    }

    String& String::operator+=(char c)
    {
        Insert(Size() - 1, c);
        return *this;
    }

    String& String::operator+=(const char* c)
    {
        Insert(Size() - 1, c);
        return *this;
    }

    String& String::operator+=(const String& str)
    {
        Insert(Size() - 1, str);
        return *this;
    }

    String& String::operator--()
    {
        if (Size() > 0)
        {
            m_Data.erase(m_Data.end() - 2);
        }
        return *this;
    }

    void String::Insert(size_t i,
                        char c)
    {
        m_Data.insert(m_Data.begin() + i, c);
    }

    void String::Insert(size_t i,
                        const char* str)
    {
        m_Data.insert_range(m_Data.begin() + i, util::PtrRange(str, strlen(str)));
    }

    void String::Insert(size_t i,
                        const String& str)
    {
        m_Data.insert_range(m_Data.begin() + i, util::PtrRange(str.Data(), str.Size()));
    }

    void String::Erase(size_t i)
    {
        m_Data.erase(m_Data.begin() + i);
    }

    size_t String::Find(char c, bool fromEnd) const
    {
        if (fromEnd)
        {
            auto it = std::find(m_Data.rbegin(), m_Data.rend(), c);
            if (it != m_Data.rend())
            {
                return std::distance(m_Data.begin(), it.base()) - 1;
            }
        }
        else
        {
            auto it = std::find(m_Data.begin(), m_Data.end(), c);
            if (it != m_Data.end())
            {
                return std::distance(m_Data.begin(), it);
            }
        }

        return InvalidPos;
    }

    String String::SubString(size_t begin, size_t end) const
    {
        if (!SPARK_VERIFY(begin < Size() && end <= Size()))
        {
            return {};
        }

        String string;
        size_t newSize = end - begin;
        string.m_Data.resize(newSize + 1);
        memcpy((void *)string.Data(), m_Data.data() + begin, newSize);
        memset((void*)(string.Data() + newSize), 0, 1);
        return string;
    }
}
