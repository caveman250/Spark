#include "String.h"

#include "engine/container_util/RangeUtil.h"
#include "engine/reflect/Reflect.h"

namespace se
{
    size_t String::InvalidPos = std::numeric_limits<size_t>::max();

    String::String()
    {
        m_Data.push_back('\0');
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
        if (m_Data.empty() || m_Data.back() != '\0')
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
        Insert(Size(), c);
        return *this;
    }

    String& String::operator+=(const char* c)
    {
        Insert(Size(), c);
        return *this;
    }

    String& String::operator+=(const String& str)
    {
        Insert(Size(), str);
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
        auto range = util::PtrRange(str, strlen(str));
        for (auto it = range.begin(); it != range.end(); ++it)
        {
            m_Data.insert(m_Data.begin() + i, *it);
            i++;
        }
    }

    void String::Insert(size_t i,
                        const String& str)
    {
        auto range = util::PtrRange(str.Data(), str.Size());
        for (auto it = range.begin(); it != range.end(); ++it)
        {
            m_Data.insert(m_Data.begin() + i, *it);
            i++;
        }
    }

    void String::Erase(size_t i)
    {
        m_Data.erase(m_Data.begin() + i);
    }

    String& String::Replace(size_t start, size_t end, const String& str)
    {
        m_Data.erase(m_Data.begin() + start, m_Data.begin() + end);
        for (auto it = str.m_Data.begin(); it != str.m_Data.end(); ++it)
        {
            m_Data.insert(m_Data.begin() + start, *it);
            start++;
        }
        return *this;
    }

    String& String::ReplaceAll(char c, char replacement)
    {
        for (size_t i = 0; i < Size(); ++i)
        {
            auto& val = m_Data[i];
            if (val == c)
            {
                val = replacement;
            }
        }

        return *this;
    }

    bool String::Contains(const String& str) const
    {
        if (str.Size() <= Size())
        {
            auto it = std::search(m_Data.begin(), m_Data.end() - 1, str.m_Data.begin(), str.m_Data.end() - 1);
            return it != m_Data.end() - 1;
        }
        return false;
    }

    bool String::StartsWith(const String& str) const
    {
        if (Size() < str.Size())
        {
            return false;
        }

        return memcmp(Data(), str.Data(), str.Size()) == 0;
    }

    bool String::EndsWith(const String& str) const
    {
        if (Size() < str.Size())
        {
            return false;
        }

        return memcmp(Data() + Size() - str.Size(), str.Data(), str.Size()) == 0;
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

    String operator+(const String &lhs,
                     const String &rhs)
    {
        String ret = lhs;
        ret.Insert(lhs.Size(), rhs);
        return ret;
    }

    String operator+(const String& lhs,
                     char rhs)
    {
        String ret = lhs;
        ret.Insert(lhs.Size(), rhs);
        return ret;
    }

    bool operator<(const String& lhs,
                     const String& rhs)
    {
        if (lhs.Size() != rhs.Size())
        {
            return lhs.Size() < rhs.Size();
        }
        for (size_t i = 0; i < lhs.Size(); ++i)
        {
            char a = lhs[i];
            char b = rhs[i];
            if (a != b)
            {
                return a < b;
            }
        }

        return false;
    }
}

std::ostream &operator<<(std::ostream &os,
                         const se::String &string)
{
    os << string.Data();
    return os;
}
