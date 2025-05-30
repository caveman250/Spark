#include "VariableDeclarationNode.h"

#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    VariableDeclarationNode::VariableDeclarationNode(const std::string &name, const Variable& var)
        : m_Var(var)
        , m_Name(name)
    {
    }

    std::string VariableDeclarationNode::GetDebugString() const
    {
        return std::format("VariableDeclarationNode - {}, {}", TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void VariableDeclarationNode::ToGlsl(ShaderCompileContext&, string::ArenaString &outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::string arrayTag = "";
        if (!m_Var.arraySizeVariable.empty())
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeVariable);
        }
        else if (m_Var.arraySizeConstant != 0)
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeConstant);
        }
        outShader.append(string::ArenaFormat("{0} {1}{2}", alloc, TypeUtil::TypeToGlsl(m_Var.type), m_Name, arrayTag));
    }

    void VariableDeclarationNode::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::string arrayTag = "";
        if (!m_Var.arraySizeVariable.empty())
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeVariable);
        }
        else if (m_Var.arraySizeConstant != 0)
        {
            arrayTag = std::format("[{}]", m_Var.arraySizeConstant);
        }
        outShader.append(string::ArenaFormat("{0} {1}{2}", alloc, TypeUtil::TypeToMtl(m_Var.type), m_Name, arrayTag));
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
