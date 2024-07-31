#include "ASTNode.h"

namespace se::asset::shader::ast
{
    DEFINE_ABSTRACT_SPARK_CLASS_BEGIN(ASTNode)
    DEFINE_SPARK_CLASS_END()

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
}
