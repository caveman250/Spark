#pragma once

#include <typeindex>

#include "Struct.h"
#include "Blob.h"
#include "Array.h"
#include "PolymorphicArray.h"

namespace se::asset::binary
{
    constexpr uint32_t s_ObjectHeaderSize = sizeof(uint32_t);

#if !SPARK_DIST
    template<typename T>
    void CheckType(Type type);
    void CheckType(Type type, const std::type_index& typeId);
#endif

    class Object
    {
    public:
        bool HasField(const std::string& field) const;

        template<typename T>
        const T Get(const std::string& field) const;

        std::string GetNativeTypeString(const std::string& field) const;

        template<typename T>
        void Set(const std::string& field, const T& val);

        void Set(uint32_t fieldIndex, const char* data, size_t size);

        const char* GetString(const std::string& field) const;
        void SetString(const std::string& field, const char* val);

        Database* GetDatabase() const { return m_DB; }

        Struct GetStruct() const { return m_Struct; }
        uint32_t GetStructIndex() const { return m_Struct.m_Index; }
        uint32_t GetOffset() const { return m_Offset; }

        nlohmann::ordered_json ToJson() const;
        void FromJson(const nlohmann::ordered_json& json);

    private:
        Object(uint32_t offset, Database* database, const Struct& objStruct);

        char* GetData() const;
        Struct GetStruct(uint32_t structIndex) const;
        Object GetObjectAt(uint32_t offset) const;
        Array GetArrayAt(uint32_t offset) const;
        PolymorphicArray GetPolymorphicArrayAt(uint32_t offset) const;
        uint32_t GetBlobOffset(const Blob& blob) const;
        Blob GetBlobAt(uint32_t offset) const;

        uint32_t m_Offset;
        Struct m_Struct;
        Database* m_DB;

        friend class Database;
        friend class Array;
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
    const T Object::Get(const std::string& field) const
    {
        uint32_t fieldIndex = m_Struct.GetFieldIndex(field);
#if !SPARK_DIST
        CheckType<T>(m_Struct.GetFieldType(fieldIndex));
#endif
        return *reinterpret_cast<T*>(GetData() + m_Struct.GetFieldOffset(fieldIndex));
    }

    // Object Specializations
    template<>
    inline void Object::Set<Object>(const std::string& field, const Object& val)
    {
#if !SPARK_DIST
        CheckType<Object>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&val.m_Offset), sizeof(uint32_t));
    }

    template<>
    inline const Object Object::Get<Object>(const std::string& field) const
    {
#if !SPARK_DIST
        CheckType<Object>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetObjectAt(offset);
    }

    // Array Specializations
    template<>
    inline void Object::Set<Array>(const std::string& field, const Array& val)
    {
#if !SPARK_DIST
        CheckType<Array>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&val.m_Offset), sizeof(uint32_t));
    }

    template<>
    inline const Array Object::Get<Array>(const std::string& field) const
    {
#if !SPARK_DIST
        CheckType<Array>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetArrayAt(offset);
    }

    template<>
    inline void Object::Set<PolymorphicArray>(const std::string& field, const PolymorphicArray& val)
    {
#if !SPARK_DIST
        CheckType<PolymorphicArray>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        Set(m_Struct.GetFieldIndex(field), reinterpret_cast<const char*>(&val.m_ArrayOffset), sizeof(uint32_t));
    }

    template<>
    inline const PolymorphicArray Object::Get<PolymorphicArray>(const std::string& field) const
    {
#if !SPARK_DIST
        CheckType<PolymorphicArray>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetPolymorphicArrayAt(offset);
    }

    // String Specializations
    template<>
    inline void Object::Set<std::string>(const std::string& field, const std::string& val)
    {
        SetString(field, val.c_str());
    }

    template<>
    inline const std::string Object::Get<std::string>(const std::string& field) const
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
    inline const Blob Object::Get<Blob>(const std::string& field) const
    {
#if !SPARK_DIST
        CheckType<Blob>(m_Struct.GetFieldType(m_Struct.GetFieldIndex(field)));
#endif
        uint32_t offset = *reinterpret_cast<uint32_t*>(GetData() + m_Struct.GetFieldOffset(m_Struct.GetFieldIndex(field)));
        return GetBlobAt(offset);
    }

#if !SPARK_DIST
    template<typename T>
    void CheckType(Type type)
    {
        CheckType(type, typeid(T));
    }
#endif
}
