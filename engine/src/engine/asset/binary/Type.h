#pragma once

namespace se::asset::binary
{
    enum class Type
    {
        Bool,
        Int8,
        Uint8,
        Int16,
        Uint16,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float,
        Double,
        Vec2,
        Vec3,
        Vec4,
        Object,
        String,
        Blob,
        Array,
        PolymorphicArray,
        Invalid
    };

    std::string TypeToString(Type type);
    Type TypeFromString(const std::string& type);
    uint32_t GetTypeSize(Type type);
}