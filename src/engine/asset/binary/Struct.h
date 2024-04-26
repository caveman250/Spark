#pragma once

#include "binary.h"

namespace se::asset::binary
{
    constexpr uint32_t s_StructHeaderSize = sizeof(uint32_t); // numFields (uint32_t)
    constexpr uint32_t s_StructRowSize = 32 + sizeof(Type); // FixedString32 + Type

    class Struct
    {
    public:
        uint32_t GetFieldCount() const;
        const char* GetFieldName(uint32_t fieldIndex) const;
        uint32_t GetFieldIndex(const std::string& name) const;
        Type GetFieldType(uint32_t fieldIndex) const;
        uint32_t CalcObjectSize() const;
        uint32_t GetFieldOffset(uint32_t fieldIndex) const;

    private:
        Struct(char* data) : m_Data(data) {}
        char* m_Data;

        friend class Asset;
    };
}
