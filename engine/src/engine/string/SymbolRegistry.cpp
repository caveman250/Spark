#include "SymbolRegistry.h"

namespace se::string
{
    uint32_t SymbolRegistry::FindOrAdd(std::string_view str)
    {
        if (str.empty()) return 0;

        {
            std::shared_lock<std::shared_mutex> readLock(m_Mutex);
            auto it = m_Lookup.find(str);
            if (it != m_Lookup.end())
            {
                return it->second;
            }
        }

        std::unique_lock<std::shared_mutex> writeLock(m_Mutex);

        auto it = m_Lookup.find(str);
        if (it != m_Lookup.end())
        {
            return it->second;
        }

        uint32_t newId = static_cast<uint32_t>(m_Strings.size());
        m_Strings.emplace_back(str);

        std::string_view pooledView = m_Strings.back();
        m_Lookup[pooledView] = newId;

        return newId;
    }

    std::string_view SymbolRegistry::GetString(uint32_t id) const
    {
        std::shared_lock<std::shared_mutex> readLock(m_Mutex);
        if (id < m_Strings.size())
        {
            return m_Strings[id];
        }
        return "";
    }

    SymbolRegistry::SymbolRegistry()
    {
        m_Strings.emplace_back("");
        m_Lookup[""] = 0;
    }
}
