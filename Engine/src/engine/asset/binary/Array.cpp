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

    Object Array::Get(size_t i)
    {
        Struct st = GetStruct();
        return Object(m_Offset + GetOffsetOf(i), m_DB, st);
    }

    Struct Array::GetStruct()
    {
        return Struct(GetStructIndex(), m_DB);
    }

    Array::Array(uint32_t offset, Database* database, bool initialiseObjects)
        : m_Offset(offset)
        , m_DB(database)
    {
        m_ElementSize = GetStruct().CalcObjectSize();
        if (initialiseObjects)
        {
            uint32_t structIndex = GetStructIndex();
            uint32_t numElements = GetCount();
            for (uint32_t i = 0; i < numElements; ++i)
            {
                char *objData = GetData() + GetOffsetOf(i);
                std::memset(objData, 0, m_ElementSize);
                std::memcpy(objData, &structIndex, sizeof(uint32_t));
            }
        }
    }

    uint32_t Array::GetOffsetOf(size_t i)
    {
        return s_ArrayHeaderSize + ((uint32_t)i * m_ElementSize);
    }

    char* Array::GetData()
    {
        return m_DB->GetObjectDataAt(m_Offset);
    }
}
