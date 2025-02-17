#include "ASTNode.h"

namespace se::asset::shader::ast
{
    DEFINE_ABSTRACT_SPARK_CLASS_BEGIN(ASTNode)
    DEFINE_SPARK_CLASS_END(ASTNode)

    ASTNode::ASTNode(const ASTNode& rhs)
    {
        m_Children.reserve(rhs.m_Children.size());
        for (const auto& child : rhs.m_Children)
        {
            auto* objBase = static_cast<ObjectBase*>(child.get());
            if (SPARK_VERIFY(objBase))
            {
                m_Children.push_back(std::shared_ptr<ASTNode>((ASTNode*)objBase->GetReflectType()->heap_copy_constructor(child.get())));
                m_Children.back()->m_Parent = this;
            }
        }
    }

    const std::shared_ptr<ASTNode>& ASTNode::AddChild(ASTNode* node)
    {
        node->m_Parent = this;
        return m_Children.emplace_back(std::shared_ptr<ASTNode>(node));
    }

    void ASTNode::DebugPrint(int indent) const
    {
        std::string str = "-";
        for (int i = 0; i < indent; ++i)
        {
            str += "\t";
        }
        str += GetDebugString();
        debug::Log::Info(str);

        for (const auto& child : m_Children)
        {
            child->DebugPrint(indent + 1);
        }
    }

    void ASTNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        for (const auto& child : m_Children)
        {
            child->CollectUsedNames(nameMap);
        }
    }

    void ASTNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        for (const auto& child : m_Children)
        {
            child->ApplyNameRemapping(newNames);
    }
        }

    void ASTNode::ForEachChild(const std::function<void(ASTNode*)>& func)
    {
        for (const auto& child : m_Children)
        {
            func(child.get());
        }
    }
}
