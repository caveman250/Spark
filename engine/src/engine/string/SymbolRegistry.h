#pragma once

#include <spark.h>
#include <shared_mutex>

namespace se::string
{
    class SymbolRegistry
    {
    public:
        static SymbolRegistry& Get()
        {
            static SymbolRegistry instance;
            return instance;
        }

        uint32_t FindOrAdd(std::string_view str);
        std::string_view GetString(uint32_t id) const;

    private:
        SymbolRegistry();

        mutable std::shared_mutex m_Mutex;
        std::vector<std::string> m_Strings;

        std::unordered_map<std::string_view, uint32_t> m_Lookup;
    };
}