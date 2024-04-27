#pragma once

namespace se::asset::binary
{
    enum class Type
    {
        Int8,
        Uint8,
        Int16,
        Uint16,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Object,
        String,
        Blob
    };

    std::string TypeToString(Type type);
    uint32_t GetTypeSize(Type type);
}