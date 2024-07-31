#include "VariableDeclarationNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(VariableDeclarationNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Type)
        DEFINE_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    VariableDeclarationNode::VariableDeclarationNode(const std::string &name, AstType::Type type)
        : m_Type(type)
        , m_Name(name)
    {
    }

    std::string VariableDeclarationNode::GetDebugString() const
    {
        return std::format("VariableDeclarationNode - {}, {}", TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void VariableDeclarationNode::ToGlsl(string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat("{0} {1}", alloc, TypeUtil::GetTypeGlsl(m_Type), m_Name));
    }

    void VariableDeclarationNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        ASTNode::CollectUsedNames(nameMap);
        if (!nameMap.contains(m_Name))
        {
            nameMap.insert({ m_Name, {} });
        }
    }

    void VariableDeclarationNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        ASTNode::ApplyNameRemapping(newNames);
        if (newNames.contains(m_Name))
        {
            m_Name = newNames.at(m_Name);
        }
    }
}
