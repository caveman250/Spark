#include "Object.h"
#include "Database.h"

namespace se::asset::binary
{
    Object::Object(char* data, Database* database, const Struct& objStruct)
        : m_Data(data)
        , m_DB(database)
        , m_Struct(objStruct)
    {

    }

    void Object::Set(uint32_t fieldIndex, const char* data, size_t size)
    {
        std::memcpy(m_Data + m_Struct.GetFieldOffset(fieldIndex), data, size);
    }

    Struct Object::GetStruct(uint32_t structIndex)
    {
        return Struct(m_DB->GetStruct(structIndex));
    }

    uint32_t Object::GetObjectOffset(const Object& obj)
    {
        return m_DB->GetObjectOffset(obj);
    }

    Object Object::GetObjectAt(uint32_t offset)
    {
        return m_DB->GetObjectAt(offset);
    }
}