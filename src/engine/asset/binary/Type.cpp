#include "Type.h"

namespace se::asset::binary
{
    std::string TypeToString(Type type)
    {
        switch (type)
        {
            case Type::Int8:
                return "Int8";
            case Type::Uint8:
                return "Uint8";
            case Type::Int16:
                return "Int16";
            case Type::Uint16:
                return "Uint16";
            case Type::Int32:
                return "Int32";
            case Type::Uint32:
                return "Uint32";
            case Type::Int64:
                return "Int64";
            case Type::Uint64:
                return "Uint64";
            case Type::Float:
                return "Float";
            case Type::Vec2:
                return "Vec2";
            case Type::Vec3:
                return "Vec3";
            case Type::Vec4:
                return "Vec4";
            case Type::Object:
                return "Object";
            case Type::String:
                return "String";
            default:
                SPARK_ASSERT(false);
                return "error";
        }
    }

    uint32_t GetTypeSize(Type type)
    {
        switch (type)
        {
            case Type::Int8:
                return sizeof(int8_t);
            case Type::Uint8:
                return sizeof(uint8_t);
            case Type::Int16:
                return sizeof(int16_t);
            case Type::Uint16:
                return sizeof(uint16_t);
            case Type::Int32:
                return sizeof(int32_t);
            case Type::Uint32:
                return sizeof(uint32_t);
            case Type::Int64:
                return sizeof(int64_t);
            case Type::Uint64:
                return sizeof(uint64_t);
            case Type::Float:
                return sizeof(float);
            case Type::Vec2:
                return sizeof(math::Vec2);
            case Type::Vec3:
                return sizeof(math::Vec3);
            case Type::Vec4:
                return sizeof(math::Vec4);
            case Type::Object:
            case Type::String:
                return sizeof(uint32_t); //offset of object data
            default:
                SPARK_ASSERT(false);
                return 0;
        }
    }
}