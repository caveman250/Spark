#pragma once

#include "Struct.h"

namespace se::asset::binary
{
    constexpr uint32_t s_ObjectHeaderSize = sizeof(uint32_t);

    class Object
    {
    public:
        template <typename T>
        const T Get(const std::string& field);

        template <typename T>
        void Set(const std::string& field, const T& val);

    private:
        template <typename T>
        void Set(uint32_t fieldIndex, const T& val);
        void Set(uint32_t fieldIndex, const char* data, size_t size);

        template <typename T>
        const T Get(uint32_t fieldIndex);

        Object(char* data, Database* database, const Struct& objStruct);
        Struct GetStruct(uint32_t structIndex);
        uint32_t GetObjectOffset(const Object& obj);
        Object GetObjectAt(uint32_t offset);

        char* m_Data;
        Struct m_Struct;
        Database* m_DB;

        friend class Database;
    };

    template<typename T>
    void Object::Set(uint32_t fieldIndex, const T& val)
    {
        Set(fieldIndex, reinterpret_cast<const char*>(&val), sizeof(T));
    }

    template<typename T>
    void Object::Set(const std::string& field, const T& val)
    {
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&val), sizeof(T));
    }

    template<typename T>
    const T Object::Get(uint32_t fieldIndex)
    {
        return *reinterpret_cast<T*>(m_Data + m_Struct.GetFieldOffset(fieldIndex));
    }

    template<typename T>
    const T Object::Get(const std::string& field)
    {
        return Get<T>(m_Struct.GetFieldIndex(field));
    }

    // Object Specializations
    template <>
    inline void Object::Set<Object>(const std::string& field, const Object& val)
    {
        uint32_t offset = GetObjectOffset(val);
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
    }

    template <>
    inline const Object Object::Get<Object>(const std::string& field)
    {
        uint32_t offset = *reinterpret_cast<uint32_t*>(m_Data + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetObjectAt(offset);
    }
}
