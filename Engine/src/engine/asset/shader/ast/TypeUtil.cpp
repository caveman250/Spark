#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    std::string TypeUtil::GetTypeGlsl(AstType::Type type)
    {
        switch (type)
        {
            case AstType::Int:
                return "int";
            case AstType::Float:
                return "float";
            case AstType::Vec2:
                return "vec2";
            case AstType::Vec3:
                return "vec3";
            case AstType::Vec4:
                return "vec4";
            case AstType::Mat3:
                return "mat3";
            case AstType::Mat4:
                return "mat4";
            case AstType::Void:
                return "void";
            case AstType::Sampler2D:
                return "sampler2D";
            default:
                SPARK_ASSERT(false);
                return "";
        }
    }

    AstType::Type TypeUtil::StringToType(const std::string& type)
    {
        if (type == "int")
            return AstType::Int;
        else if (type == "float")
            return AstType::Float;
        else if (type == "vec2")
            return AstType::Vec2;
        else if (type == "vec3")
            return AstType::Vec3;
        else if (type == "vec4")
            return AstType::Vec4;
        else if (type == "mat3")
            return AstType::Mat3;
        else if (type == "mat4")
            return AstType::Mat4;
        else if (type == "void")
            return AstType::Void;
        else if (type == "sampler2D")
            return AstType::Sampler2D;

        return AstType::Invalid;
    }

    const std::vector<std::string>& TypeUtil::GetTypeStrings()
    {
        static std::vector<std::string> s_TypeStrings =
        {
            "int",
            "float",
            "vec2",
            "vec3",
            "vec4",
            "mat3",
            "mat4",
            "void",
            "sampler2D"
        };

        return s_TypeStrings;
    }
}
