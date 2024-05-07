#pragma once

#include "binary.h"

namespace se::asset::binary
{
    class Database;
}

namespace se::asset::binary
{
    constexpr uint32_t s_StructHeaderSize = sizeof(uint32_t); // numFields (uint32_t)
    constexpr uint32_t s_StructRowSize = 32 + sizeof(Type); // FixedString32 + Type

    class Struct
    {
    public:
        static Struct Invalid;

        uint32_t GetFieldCount() const;
        const char* GetFieldName(uint32_t fieldIndex) const;
        uint32_t GetFieldIndex(const std::string& name) const;
        Type GetFieldType(uint32_t fieldIndex) const;
        uint32_t CalcObjectSize() const;
        uint32_t GetFieldOffset(uint32_t fieldIndex) const;

    private:
        Struct(uint32_t index, Database* db) : m_Index(index), m_DB(db) {}
        uint32_t m_Index;
        Database* m_DB;

        char* GetData() const;

        friend class Database;
        friend class Object;
        friend class Array;
    };
}
