#include "ASTNode.h"

namespace se::shader::ast
{
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
