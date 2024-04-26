#pragma once

#include "Struct.h"
#include "Blob.h"

namespace se::asset::binary
{
    constexpr uint32_t s_ObjectHeaderSize = sizeof(uint32_t);

#if !SPARK_DIST
    template<typename T>
    void CheckType(Type type);
#endif

    class Object
    {
    public:
        template<typename T>
        const T Get(const std::string& field);

        template<typename T>
        void Set(const std::string& field, const T& val);

        const char* GetString(const std::string& field);
        void SetString(const std::string& field, const char* val);

    private:
        void Set(uint32_t fieldIndex, const char* data, size_t size);

        Object(char* data, Database* database, const Struct& objStruct);

        Struct GetStruct(uint32_t structIndex);

        uint32_t GetObjectOffset(const Object& obj);
        Object GetObjectAt(uint32_t offset);
        uint32_t GetBlobOffset(const Blob& blob);
        Blob GetBlobAt(uint32_t offset);

        char* m_Data;
        Struct m_Struct;
        Database* m_DB;

        friend class Database;
    };

    template<typename T>
    void Object::Set(const std::string& field, const T& val)
    {
#if !SPARK_DIST
        CheckType<T>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&val), sizeof(T));
    }

    template<typename T>
    const T Object::Get(const std::string& field)
    {
        uint32_t fieldIndex = m_Struct.GetFieldIndex(field);
#if !SPARK_DIST
        CheckType<T>(m_Struct.GetFieldType(fieldIndex));
#endif
        return *reinterpret_cast<T*>(m_Data + m_Struct.GetFieldOffset(fieldIndex));
    }

    // Object Specializations
    template<>
    inline void Object::Set<Object>(const std::string& field, const Object& val)
    {
#if !SPARK_DIST
        CheckType<Object>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = GetObjectOffset(val);
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
    }

    template<>
    inline const Object Object::Get<Object>(const std::string& field)
    {
#if !SPARK_DIST
        CheckType<Object>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(m_Data + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetObjectAt(offset);
    }

    // String Specializations
    template<>
    inline void Object::Set<std::string>(const std::string& field, const std::string& val)
    {
        SetString(field, val.c_str());
    }

    template<>
    inline const std::string Object::Get<std::string>(const std::string& field)
    {
        return GetString(field);
    }

    // Blob Specializations
    template<>
    inline void Object::Set<Blob>(const std::string& field, const Blob& val)
    {
#if !SPARK_DIST
        CheckType<Blob>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = GetBlobOffset(val);
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
    }

    template<>
    inline const Blob Object::Get<Blob>(const std::string& field)
    {
#if !SPARK_DIST
        CheckType<Blob>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(m_Data + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetBlobAt(offset);
    }

#if !SPARK_DIST
    template<typename T>
    void CheckType(Type type)
    {
        switch (type)
        {
            case Type::Int8:
                SPARK_ASSERT(typeid(T) == typeid(int8_t));
                break;
            case Type::Uint8:
                SPARK_ASSERT(typeid(T) == typeid(uint8_t));
                break;
            case Type::Int16:
                SPARK_ASSERT(typeid(T) == typeid(int16_t));
                break;
            case Type::Uint16:
                SPARK_ASSERT(typeid(T) == typeid(uint16_t));
                break;
            case Type::Int32:
                SPARK_ASSERT(typeid(T) == typeid(int32_t));
                break;
            case Type::Uint32:
                SPARK_ASSERT(typeid(T) == typeid(uint32_t));
                break;
            case Type::Int64:
                SPARK_ASSERT(typeid(T) == typeid(int64_t));
                break;
            case Type::Uint64:
                SPARK_ASSERT(typeid(T) == typeid(uint64_t));
                break;
            case Type::Float:
                SPARK_ASSERT(typeid(T) == typeid(float));
                break;
            case Type::Vec2:
                SPARK_ASSERT(typeid(T) == typeid(math::Vec2));
                break;
            case Type::Vec3:
                SPARK_ASSERT(typeid(T) == typeid(math::Vec3));
                break;
            case Type::Vec4:
                SPARK_ASSERT(typeid(T) == typeid(math::Vec4));
                break;
            case Type::Object:
                SPARK_ASSERT(typeid(T) == typeid(Object));
                break;
            case Type::String:
                SPARK_ASSERT(typeid(T) == typeid(const char*));
                break;
            case Type::Blob:
                SPARK_ASSERT(typeid(T) == typeid(Blob));
                break;
            default:
                SPARK_ASSERT(false, "CheckType - Unrecognized type!");
                break;
        }
    }
#endif
}
