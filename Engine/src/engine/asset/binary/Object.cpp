#include "Object.h"
#include "Database.h"

namespace se::asset::binary
{
    Object::Object(uint32_t offset, Database* database, const Struct& objStruct)
        : m_Offset(offset)
        , m_DB(database)
        , m_Struct(objStruct)
    {

    }

    char* Object::GetData()
    {
        return m_DB->GetObjectDataAt(m_Offset);
    }

    void Object::Set(uint32_t fieldIndex, const char* data, size_t size)
    {
        std::memcpy(GetData() + m_Struct.GetFieldOffset(fieldIndex), data, size);
    }

    Struct Object::GetStruct(uint32_t structIndex)
    {
        return Struct(m_DB->GetStruct(structIndex));
    }

    Object Object::GetObjectAt(uint32_t offset)
    {
        return m_DB->GetObjectAt(offset);
    }

    const char* Object::GetString(const std::string& field)
    {
#if !SPARK_DIST
        CheckType<const char*>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return m_DB->GetStringAt(offset);
    }

    void Object::SetString(const std::string& field, const char* val)
    {
#if !SPARK_DIST
        CheckType<const char*>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif

        uint32_t offset = m_DB->CreateString(val);
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
    }

    uint32_t Object::GetBlobOffset(const Blob& blob)
    {
        return m_DB->GetBlobOffset(blob);
    }

    Blob Object::GetBlobAt(uint32_t offset)
    {
        return m_DB->GetBlobAt(offset);
    }
}