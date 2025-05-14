#include "TypeUtil.h"

#include "engine/math/math.h"

namespace se::asset::shader::ast
{
    std::string TypeUtil::TypeToGlsl(AstType::Type type)
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
            case AstType::Bool:
                return "bool";
            default:
                SPARK_ASSERT(false);
                return "";
        }
    }

    AstType::Type TypeUtil::GlslToType(const std::string& type)
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
        else if (type == "bool")
            return AstType::Bool;

        return AstType::Invalid;
    }

    std::string TypeUtil::TypeToMtl(AstType::Type type)
    {
        switch (type)
        {
            case AstType::Int:
                return "int";
            case AstType::Float:
                return "float";
            case AstType::Vec2:
                return "float2";
            case AstType::Vec3:
                return "float3";
            case AstType::Vec4:
                return "float4";
            case AstType::Mat3:
                return "float3x3";
            case AstType::Mat4:
                return "float4x4";
            case AstType::Void:
                return "void";
            case AstType::Sampler2D:
                return "texture2d";
            case AstType::Bool:
                return "bool";
            default:
                SPARK_ASSERT(false);
                return "";
        }
    }

    AstType::Type TypeUtil::MtlToType(const std::string& type)
    {
        if (type == "int")
            return AstType::Int;
        else if (type == "float")
            return AstType::Float;
        else if (type == "float2")
            return AstType::Vec2;
        else if (type == "float3")
            return AstType::Vec3;
        else if (type == "float4")
            return AstType::Vec4;
        else if (type == "float3x3")
            return AstType::Mat3;
        else if (type == "float4x4")
            return AstType::Mat4;
        else if (type == "void")
            return AstType::Void;
        else if (type == "texture2D")
            return AstType::Sampler2D;
        else if (type == "bool")
            return AstType::Bool;

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

    size_t TypeUtil::GetTypeSize(AstType::Type type)
    {
        switch (type)
        {
            case AstType::Bool:
                return sizeof(bool);
            case AstType::Int:
                return sizeof(int32_t);
            case AstType::Float:
                return sizeof(float);
            case AstType::Vec2:
                return sizeof(math::Vec2);
            case AstType::Vec3:
                return sizeof(math::Vec3);
            case AstType::Vec4:
                return sizeof(math::Vec4);
            case AstType::Mat3:
                return sizeof(math::Mat3);
            case AstType::Mat4:
                return sizeof(math::Mat4);
            case AstType::Void:
                return 0;
            case AstType::Sampler2D:
                return 0;
            default:
                SPARK_ASSERT(false);
                return 0;
        }
    }

    size_t TypeUtil::GetTypePaddedSize(AstType::Type type)
    {
        switch (type)
        {
            case AstType::Bool:
                return sizeof(bool);
            case AstType::Int:
                return sizeof(int32_t);
            case AstType::Float:
                return sizeof(float);
            case AstType::Vec2:
                return sizeof(math::Vec2);
            case AstType::Vec3:
                return sizeof(math::Vec4);
            case AstType::Vec4:
                return sizeof(math::Vec4);
            case AstType::Mat3:
                return sizeof(math::Mat3);
            case AstType::Mat4:
                return sizeof(math::Mat4);
            case AstType::Void:
                return 0;
            case AstType::Sampler2D:
                return 0;
            default:
                SPARK_ASSERT(false);
                return 0;
        }
    }
}
