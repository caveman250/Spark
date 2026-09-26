#include "Symbol.h"

namespace se::string
{
    constexpr bool Symbol::operator==(const Symbol& other) const noexcept
    {
        return m_ID == other.m_ID;
    }

    constexpr bool Symbol::operator!=(const Symbol& other) const noexcept
    {
        return m_ID != other.m_ID;
    }

    constexpr bool Symbol::operator<(const Symbol& other) const noexcept
    {
        return m_ID < other.m_ID;
    }

    constexpr bool Symbol::IsNone() const noexcept
    {
        return m_ID == 0;
    }

    std::string_view Symbol::ToStringView() const
    {
        return SymbolRegistry::Get().GetString(m_ID);
    }

    const char* Symbol::c_str() const
    {
        return SymbolRegistry::Get().GetString(m_ID).data();
    }
}
