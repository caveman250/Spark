#include "TypeUtil.h"

namespace se::shader::ast
{
    std::string TypeUtil::GetTypeGlsl(Type type)
    {
        switch (type)
        {
            case Type::Float:
                return "float";
            case Type::Vec2:
                return "vec2";
            case Type::Vec3:
                return "vec3";
            case Type::Vec4:
                return "vec4";
            case Type::Mat3:
                return "mat3";
            case Type::Mat4:
                return "mat4";
            case Type::Void:
                return "void";
            default:
                SPARK_ASSERT(false);
                return "";
        }
    }

    Type TypeUtil::StringToType(const std::string& type)
    {
        if (type == "float")
            return Type::Float;
        else if (type == "vec2")
            return Type::Vec2;
        else if (type == "vec3")
            return Type::Vec3;
        else if (type == "vec4")
            return Type::Vec4;
        else if (type == "mat3")
            return Type::Mat3;
        else if (type == "mat4")
            return Type::Mat4;
        else if (type == "void")
            return Type::Void;

        SPARK_ASSERT(false);
        return Type::Invalid;
    }
}
