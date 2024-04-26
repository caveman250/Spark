#pragma once

#include "Struct.h"

namespace se::asset::binary
{
    constexpr uint32_t s_ObjectHeaderSize = sizeof(uint32_t);

    class Object
    {
    public:
        template <typename T>
        const T& Get(const std::string& field);
        template <typename T>
        const T& Get(uint32_t fieldIndex);
        template <typename T>
        void Set(const std::string& field, const T& val);
        template <typename T>
        void Set(uint32_t fieldIndex, const T& val);
        void Set(uint32_t fieldIndex, const char* data, size_t size);
    private:
        Object(char* data, const Struct& objStruct);

        char* m_Data;
        Struct m_Struct;

        friend class Asset;
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
    const T& Object::Get(uint32_t fieldIndex)
    {
        return *reinterpret_cast<T*>(m_Data + m_Struct.GetFieldOffset(fieldIndex));
    }

    template<typename T>
    const T& Object::Get(const std::string& field)
    {
        return Get<T>(m_Struct.GetFieldIndex(field));
    }
}
