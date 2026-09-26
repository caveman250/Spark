#include "Symbol.h"

namespace se::string
{
    constexpr bool Symbol::IsNone() const noexcept
    {
        return m_ID == 0;
    }

    std::string_view Symbol::ToStringView() const
    {
        return SymbolRegistry::Get().GetString(m_ID);
    }

    std::string Symbol::ToString() const
    {
        return std::string(SymbolRegistry::Get().GetString(m_ID));
    }

    const char* Symbol::c_str() const
    {
        return SymbolRegistry::Get().GetString(m_ID).data();
    }
}
