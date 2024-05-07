#pragma once
#include "Struct.h"

namespace se::asset::binary
{
    constexpr uint32_t s_ArrayHeaderSize = sizeof(uint32_t) * 2; // length, structindex

    class Array
    {
    public:
        uint32_t GetCount();
        Struct GetStruct();
        uint32_t GetStructIndex();
        Object Get(uint32_t i);
    private:
        Array(uint32_t offset, Database* database, bool initialiseObjects);
        uint32_t GetOffsetOf(uint32_t i);
        char* GetData();

        uint32_t m_Offset;
        Database* m_DB;
        uint32_t m_ElementSize; //cache

        friend class Database;
        friend class Object;
    };
}
