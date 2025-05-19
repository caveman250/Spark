#pragma once
#include "spark.h"
#include "ASTNode.h"
#include "engine/math/math.h"

namespace se::asset::shader::ast
{
    typedef std::variant<int, float, math::Vec2, math::Vec3, math::Vec4> ShaderValue;
    class ConstantNodeBase : public ASTNode
    {
    public:
        virtual ShaderValue GetValue() = 0;
    };

    template <typename T>
    class ConstantNode : public ConstantNodeBase
    {
        SPARK_CLASS_TEMPLATED()
    public:
        ConstantNode() : ConstantNodeBase() {}
        ConstantNode(T t);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        ShaderValue GetValue() override;
    private:

        SPARK_MEMBER(Serialized)
        T m_Constant = {};
    };

    SPARK_INSTANTIATE_TEMPLATE(ConstantNode, int);
    SPARK_INSTANTIATE_TEMPLATE(ConstantNode, float);
    SPARK_INSTANTIATE_TEMPLATE(ConstantNode, math::Vec2);
    SPARK_INSTANTIATE_TEMPLATE(ConstantNode, math::Vec3);
    SPARK_INSTANTIATE_TEMPLATE(ConstantNode, math::Vec4);

    template <typename T>
    ConstantNode<T>::ConstantNode(T t)
        : ConstantNodeBase()
    {
        m_Constant = t;
    }

    template <typename T>
    std::string ConstantNode<T>::GetDebugString() const
    {
        return std::format("ConstantNode - {}", m_Constant);
    }

    template <typename T>
    void ConstantNode<T>::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("{}", alloc, m_Constant);
    }

    template<typename T>
    void ConstantNode<T>::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("{}", alloc, m_Constant);
    }

    template<>
    inline void ConstantNode<float>::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        if (m_Constant - static_cast<int>(m_Constant) == 0.f)
        {
            outShader += string::ArenaFormat("{}.0f", alloc, m_Constant);
        }
        else
        {
            outShader += string::ArenaFormat("{}f", alloc, m_Constant);
        }
    }

    template<>
    inline void ConstantNode<math::Vec2>::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("float2({}, {})", alloc, m_Constant.x, m_Constant.y);
    }

    template<>
    inline void ConstantNode<math::Vec3>::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("float3({}, {}, {})", alloc, m_Constant.x, m_Constant.y, m_Constant.z);
    }

    template<>
    inline void ConstantNode<math::Vec4>::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("float4({}, {}, {}, {})", alloc, m_Constant.x, m_Constant.y, m_Constant.z, m_Constant.w);
    }

    template<typename T>
    ShaderValue ConstantNode<T>::GetValue()
    {
        return m_Constant;
    }
}

#include "se_asset_shader_ast_ConstantNode.generated.h"
