#include "IfNode.h"

#include "BinaryExpressionNode.h"
#include "ConstantNode.h"
#include "VariableDeclarationNode.h"
#include "VariableReferenceNode.h"
#include "engine/math/math.h"
#include "ShaderCompileContext.h"

namespace se::asset::shader::ast
{
    IfNode::IfNode(const IfNode& rhs)
            : ASTNode(rhs)
    {
        if (rhs.m_Condition)
        {
            auto* objBase = static_cast<ObjectBase *>(rhs.m_Condition.get());
            m_Condition = std::shared_ptr<ASTNode>((ASTNode *) objBase->GetReflectType()->heap_copy_constructor(rhs.m_Condition.get()));
        }
    }

    void IfNode::ApplyNameRemapping(const std::map<std::string, std::string>& newNames)
    {
        m_Condition->ApplyNameRemapping(newNames);
        ASTNode::ApplyNameRemapping(newNames);
    }

    void IfNode::ForEachChild(const std::function<void(ASTNode*)>& func)
    {
        func(m_Condition.get());
        ASTNode::ForEachChild(func);
    }

    const std::shared_ptr<ASTNode>& IfNode::AddChild(ASTNode* node)
    {
        if (!m_Condition)
        {
            return m_Condition = std::shared_ptr<ASTNode>(node);
        }
        else
        {
            return ASTNode::AddChild(node);
        }
    }

    std::string IfNode::GetDebugString() const
    {
        return "IfNode";
    }

    void IfNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += "if(";
        m_Condition->ToGlsl(context, outShader);
        outShader += ")\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToGlsl(context, outShader);
        }

        outShader += "}\n";
    }

    void IfNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += "if(";
        m_Condition->ToMtl(context, outShader);
        outShader += ")\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }

        outShader += "}\n";
    }
}
