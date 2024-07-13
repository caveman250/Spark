#include "ASTNode.h"

namespace se::asset::shader::ast
{
    void ASTNode::DebugPrint(int indent) const
    {
        std::string str = "-";
        for (int i = 0; i < indent; ++i)
        {
            str += "\t";
        }
        str += GetDebugString();
        debug::Log::Info(str);

        for (auto* child : m_Children)
        {
            child->DebugPrint(indent + 1);
        }
    }

    void ASTNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        for (auto* child : m_Children)
        {
            child->CollectUsedNames(nameMap);
        }
    }

    void ASTNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        for (auto* child : m_Children)
        {
            child->ApplyNameRemapping(newNames);
        }
    }
}
