#include "VariableReferenceNode.h"

#include "InputAttributeNode.h"
#include "NameGenerator.h"
#include "OutputNode.h"
#include "../Shader.h"

namespace se::asset::shader::ast
{
    VariableReferenceNode::VariableReferenceNode(const std::string& name, const Shader& shaderStageAst)
    {
        SPARK_ASSERT(name != " ");
        Type type;
        if (shaderStageAst.FindVariable(name, type))
        {
            m_Type = type;
            m_Name = name;
        }
        else
        {
            debug::Log::Error("ShaderCompiler: VariableReferenceNode cannot find variable with name {0}", name);
        }
    }

    std::string VariableReferenceNode::GetDebugString() const
    {
        return std::format("VariableReferenceNode - {}", m_Name);
    }

    void VariableReferenceNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += m_Name;
    }

    void VariableReferenceNode::CollectUsedNames(std::map<std::string, std::string> &nameMap) const
    {
        ASTNode::CollectUsedNames(nameMap);
        if (!nameMap.contains(m_Name))
        {
            nameMap.insert({ m_Name, {} });
        }
    }

    void VariableReferenceNode::ApplyNameRemapping(const std::map<std::string, std::string> &newNames)
    {
        ASTNode::ApplyNameRemapping(newNames);
        if (newNames.contains(m_Name))
        {
            m_Name = newNames.at(m_Name);
        }
    }
}
