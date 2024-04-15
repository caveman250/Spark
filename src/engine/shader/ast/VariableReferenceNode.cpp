#include "VariableReferenceNode.h"

#include "InputAttributeNode.h"
#include "NameGenerator.h"
#include "OutputNode.h"
#include "ShaderStage.h"
#include "engine/logging/Log.h"

namespace se::shader::ast
{
    VariableReferenceNode::VariableReferenceNode(const std::string& name, const ShaderStage& shaderStageAst)
    {
        Type type;
        if (shaderStageAst.FindVariable(name, type))
        {
            m_Type = type;
            m_Name = name;
        }
        else
        {
            logging::Log::Error("ShaderCompiler: VariableReferenceNode cannot find variable with name %s", name);
        }
    }

    void VariableReferenceNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += m_Name;
    }
}
