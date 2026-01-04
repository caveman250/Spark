#include "Type.h"
#include "engine/math/math.h"

namespace se::asset::binary
{
    std::string TypeToString(Type type)
    {
        switch (type)
        {
            case Type::Bool:
                return "bool";
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
            case Type::Double:
                return "Double";
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
            case Type::Blob:
                return "Blob";
            case Type::Array:
                return "Array";
            case Type::PolymorphicArray:
                return "PolymorphicArray";
            default:
                SPARK_ASSERT(false);
                return "error";
        }
    }

    Type TypeFromString(const std::string& type)
    {
        if (type == "bool")
        {
            return Type::Bool;
        }
        if (type == "Int8")
        {
            return Type::Int8;
        }
        if (type == "Uint8")
        {
            return Type::Uint8;
        }
        if (type == "Int16")
        {
            return Type::Int16;
        }
        if (type == "Uint16")
        {
            return Type::Uint16;
        }
        if (type == "Int32")
        {
            return Type::Int32;
        }
        if (type == "Uint32")
        {
            return Type::Uint32;
        }
        if (type == "Int64")
        {
            return Type::Int64;
        }
        if (type == "Uint64")
        {
            return Type::Uint64;
        }
        if (type == "Float")
        {
            return Type::Float;
        }
        if (type == "Double")
        {
            return Type::Double;
        }
        if (type == "Vec2")
        {
            return Type::Vec2;
        }
        if (type == "Vec3")
        {
            return Type::Vec3;
        }
        if (type == "Vec4")
        {
            return Type::Vec4;
        }
        if (type == "Object")
        {
            return Type::Object;
        }
        if (type == "String")
        {
            return Type::String;
        }
        if (type == "Blob")
        {
            return Type::Blob;
        }
        if (type == "Array")
        {
            return Type::Array;
        }
        if (type == "PolymorphicArray")
        {
            return Type::PolymorphicArray;
        }

        SPARK_ASSERT(false);
        return Type::Invalid;
    }

    uint32_t GetTypeSize(Type type)
    {
        switch (type)
        {
            case Type::Bool:
                return sizeof(bool);
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
            case Type::Double:
                return sizeof(double);
            case Type::Vec2:
                return sizeof(math::Vec2);
            case Type::Vec3:
                return sizeof(math::Vec3);
            case Type::Vec4:
                return sizeof(math::Vec4);
            case Type::String:
                return sizeof(uint64_t);
            case Type::Object:
            case Type::Blob:
            case Type::Array:
            case Type::PolymorphicArray:
                return sizeof(uint32_t); //offset of object data
            default:
                SPARK_ASSERT(false);
                return 0;
        }
    }
}