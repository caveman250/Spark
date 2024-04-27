#pragma once
#include "spark.h"
#include "ASTNode.h"

namespace se::shader::ast
{
    template <typename T>
    class ConstantNode : public ASTNode
    {
    public:
        ConstantNode(T t);
        void ToGlsl(string::ArenaString& outShader) const override;
    private:
        T m_Constant;
    };

    template <typename T>
    ConstantNode<T>::ConstantNode(T t)
    {
        m_Constant = t;
    }

    template <typename T>
    void ConstantNode<T>::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("{}", alloc, m_Constant);
    }
}
