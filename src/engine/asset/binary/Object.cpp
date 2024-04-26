#include "Object.h"

namespace se::asset::binary
{
    Object::Object(char* data, const Struct& objStruct)
        : m_Data(data)
        , m_Struct(objStruct)
    {

    }

    void Object::Set(uint32_t fieldIndex, const char* data, size_t size)
    {
        std::memcpy(m_Data + m_Struct.GetFieldOffset(fieldIndex), data, size);
    }
}