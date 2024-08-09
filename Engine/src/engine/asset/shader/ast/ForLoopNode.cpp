#include "ForLoopNode.h"

#include "VariableDeclarationNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(ForLoopNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Declaration)
        DEFINE_MEMBER(m_Condition)
        DEFINE_MEMBER(m_Expression)
    DEFINE_SPARK_CLASS_END()

    ForLoopNode::ForLoopNode(const ForLoopNode& rhs)
        : ASTNode(rhs)
    {
        m_Declaration.reserve(rhs.m_Declaration.size());
        for (const auto& child : rhs.m_Declaration)
        {
            auto* objBase = static_cast<ObjectBase*>(child.get());
            if (SPARK_VERIFY(objBase))
            {
                m_Declaration.push_back(std::shared_ptr<ASTNode>((ASTNode*)objBase->GetReflectType()->heap_copy_constructor(child.get())));
            }
        }

        m_Condition.reserve(rhs.m_Condition.size());
        for (const auto& child : rhs.m_Condition)
        {
            auto* objBase = static_cast<ObjectBase*>(child.get());
            if (SPARK_VERIFY(objBase))
            {
                m_Condition.push_back(std::shared_ptr<ASTNode>((ASTNode*)objBase->GetReflectType()->heap_copy_constructor(child.get())));
            }
        }

        m_Expression.reserve(rhs.m_Expression.size());
        for (const auto& child : rhs.m_Expression)
        {
            auto* objBase = static_cast<ObjectBase*>(child.get());
            if (SPARK_VERIFY(objBase))
            {
                m_Expression.push_back(std::shared_ptr<ASTNode>((ASTNode*)objBase->GetReflectType()->heap_copy_constructor(child.get())));
            }
        }
    }

    void ForLoopNode::ApplyNameRemapping(const std::map<std::string, std::string>& newNames)
    {
        std::ranges::for_each(m_Declaration, [newNames](const auto& item) { item->ApplyNameRemapping(newNames); });
        std::ranges::for_each(m_Condition, [newNames](const auto& item) { item->ApplyNameRemapping(newNames); });
        std::ranges::for_each(m_Expression, [newNames](const auto& item) { item->ApplyNameRemapping(newNames); });
        ASTNode::ApplyNameRemapping(newNames);
    }

    const std::shared_ptr<ASTNode>& ForLoopNode::AddChild(ASTNode* node)
    {
        if (!m_DeclarationEnded)
        {
            return m_Declaration.emplace_back(std::shared_ptr<ASTNode>(node));
        }
        else if (!m_ConditionEnded)
        {
            return m_Condition.emplace_back(std::shared_ptr<ASTNode>(node));
        }
        else if (!m_ExpressionEnded)
        {
            return m_Expression.emplace_back(std::shared_ptr<ASTNode>(node));
        }
        else
        {
            return ASTNode::AddChild(node);
        }
    }

    std::string ForLoopNode::GetDebugString() const
    {
        return "ForLoopNode";
    }

    void ForLoopNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::ranges::for_each(m_Declaration, [&outShader](const auto& item) { item->ToGlsl(outShader); });
        outShader += ";\nfor(;";
        std::ranges::for_each(m_Condition, [&outShader](const auto& item) { item->ToGlsl(outShader); });
        outShader += ";";
        std::ranges::for_each(m_Expression, [&outShader](const auto& item) { item->ToGlsl(outShader); });
        outShader += ")\n{\n";

        for (const auto& child : m_Children)
        {
            child->ToGlsl(outShader);
        }

        outShader += "}\n";
    }
}
