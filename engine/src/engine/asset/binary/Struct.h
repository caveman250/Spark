#pragma once

#include "binary.h"

namespace se::asset::binary
{
    class Database;
}

namespace se::asset::binary
{
    constexpr uint32_t s_StructHeaderSize = sizeof(uint32_t) + 64; // numFields (uint32_t) name (FixedString64)
    constexpr uint32_t s_StructRowSize = 64 + sizeof(Type); // FixedString64 + Type

    class Struct
    {
    public:
        static Struct Invalid;

        const std::string& GetName() const;
        uint32_t GetFieldCount() const;
        const char* GetFieldName(uint32_t fieldIndex) const;
        uint32_t GetFieldIndex(const std::string& name) const;
        Type GetFieldType(uint32_t fieldIndex) const;
        uint32_t CalcObjectSize() const;
        uint32_t GetFieldOffset(uint32_t fieldIndex) const;

        static void FromJson(Database* db, const nlohmann::json& json);
        nlohmann::json ToJson() const;

    private:
        Struct(const std::string& name, uint32_t index, Database* db) : m_Name(name), m_Index(index), m_DB(db) {}
        std::string m_Name;
        uint32_t m_Index;
        Database* m_DB;

        char* GetData() const;

        friend class Database;
        friend class Object;
        friend class Array;
    };
}
