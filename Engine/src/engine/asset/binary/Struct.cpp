#include "Struct.h"
#include "Object.h"

namespace se::asset::binary
{
    Struct Struct::Invalid = Struct(std::numeric_limits<uint32_t>().max(), nullptr);

    uint32_t Struct::GetFieldCount() const
    {
        return *(reinterpret_cast<uint32_t*>(GetData()));
    }

    const char* Struct::GetFieldName(uint32_t fieldIndex) const
    {
        uint32_t offset = s_StructHeaderSize + (s_StructRowSize * fieldIndex);
        char* name = GetData() + offset;
        return name;
    }

    Type Struct::GetFieldType(uint32_t fieldIndex) const
    {
        uint32_t offset = s_StructHeaderSize + (s_StructRowSize * fieldIndex) + 32;
        Type* name = reinterpret_cast<Type*>(GetData() + offset);
        return *name;
    }

    uint32_t Struct::CalcObjectSize() const
    {
        return GetFieldOffset(GetFieldCount());
    }

    uint32_t Struct::GetFieldOffset(uint32_t fieldIndex) const
    {
        uint32_t offset = s_ObjectHeaderSize;
        for (uint32_t i = 0; i < fieldIndex; ++i)
        {
            offset += GetTypeSize(GetFieldType(i));
        }

        return offset;
    }

    char* Struct::GetData() const
    {
        return m_DB->GetStructData(m_Index);
    }

    uint32_t Struct::GetFieldIndex(const std::string& name) const
    {
        for (uint32_t i = 0; i < GetFieldCount(); ++i)
        {
            if (name == GetFieldName(i))
            {
                return i;
            }
        }

        SPARK_ASSERT(false);
        return std::numeric_limits<uint32_t>().max();
    }
}