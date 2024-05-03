#include "Array.h"

#include "Database.h"

namespace se::asset::binary
{
    uint32_t Array::GetCount()
    {
        return *reinterpret_cast<uint32_t*>(GetData());
    }

    uint32_t Array::GetStructIndex()
    {
        return  *(reinterpret_cast<uint32_t*>(GetData()) + 1);
    }

    Object Array::Get(uint32_t i)
    {
        Struct st = GetStruct();
        return Object(m_Offset + GetOffsetOf(i), m_DB, st);
    }

    Struct Array::GetStruct()
    {
        return m_DB->GetStruct(GetStructIndex());
    }

    Array::Array(uint32_t offset, Database* database)
        : m_Offset(offset)
        , m_DB(database)
    {
        uint32_t structIndex = GetStructIndex();
        uint32_t numElements = GetCount();
        m_ElementSize = GetStruct().CalcObjectSize();
        for (uint32_t i = 0; i < numElements; ++i)
        {
            char* objData = GetData() + GetOffsetOf(i);
            std::memset(objData, 0, m_ElementSize);
            std::memcpy(objData, &structIndex, sizeof(uint32_t));
        }
    }

    uint32_t Array::GetOffsetOf(uint32_t i)
    {
        return s_ArrayHeaderSize + (i * m_ElementSize);
    }

    char* Array::GetData()
    {
        return m_DB->GetObjectDataAt(m_Offset);
    }
}
