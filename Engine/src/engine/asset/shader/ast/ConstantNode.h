#pragma once
#include "spark.h"
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    template <typename T>
    class ConstantNode : public ASTNode
    {
        DECLARE_SPARK_CLASS_TEMPLATED(ConstantNode, T)
    public:
        ConstantNode() {}
        ConstantNode(T t);
        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
    private:
        T m_Constant = {};
    };

    template <typename T>
    ConstantNode<T>::ConstantNode(T t)
    {
        m_Constant = t;
    }

    template <typename T>
    std::string ConstantNode<T>::GetDebugString() const
    {
        return std::format("ConstantNode - {}", m_Constant);
    }

    template <typename T>
    void ConstantNode<T>::ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("{}", alloc, m_Constant);
    }

    template<typename T>
    void ConstantNode<T>::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("{}", alloc, m_Constant);
    }

    DEFINE_SPARK_CLASS_TEMPLATED_BEGIN(ConstantNode, TEMPLATE_TYPES(T), TEMPLATE_PARAMETERS(typename T))
            DEFINE_SERIALIZED_MEMBER_TEMPLATED(ConstantNode, m_Children, TEMPLATE_TYPES(T))
        DEFINE_SERIALIZED_MEMBER_TEMPLATED(ConstantNode, m_Constant, TEMPLATE_TYPES(T))
    DEFINE_SPARK_CLASS_END()
}
