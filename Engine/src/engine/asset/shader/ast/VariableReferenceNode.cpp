#include "VariableReferenceNode.h"

#include "InputAttributeNode.h"
#include "NameGenerator.h"
#include "OutputNode.h"
#include "ShaderCompileContext.h"
#include "../Shader.h"

namespace se::asset::shader::ast
{
    VariableReferenceNode::VariableReferenceNode(const std::string& name, const Shader& shaderStageAst, int index, const std::string& indexVar)
    {
        SPARK_ASSERT(name != " ");
        AstType::Type type;
        if (shaderStageAst.FindVariable(name, type))
        {
            m_Type = type;
            m_Name = name;
            m_Index = index;
            m_IndexVar = indexVar;
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

    void VariableReferenceNode::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += m_Name;
        if (m_Index >= 0)
        {
            auto alloc = outShader.get_allocator();
            outShader += string::ArenaFormat("[{}]", alloc, m_Index);
        }
        else if (!m_IndexVar.empty())
        {
            auto alloc = outShader.get_allocator();
            outShader += string::ArenaFormat("[{}]", alloc, m_IndexVar);
        }
    }

    bool VariableReferenceNode::IsVertexInput(ShaderCompileContext& context) const
    {
        if (context.currentShader->GetType() == ShaderType::Vertex)
        {
            if (context.currentShader->FindInput(m_Name))
            {
                return true;
            }
        }

        return false;
    }

    bool VariableReferenceNode::IsVertexOutput(ShaderCompileContext& context) const
    {
        if (context.currentShader->GetType() == ShaderType::Vertex)
        {
            if (context.currentShader->FindOutput(m_Name) || context.vertexPositionOutputName == m_Name)
            {
                return true;
            }
        }

        return false;
    }

    bool VariableReferenceNode::IsFragmentInput(ShaderCompileContext& context) const
    {
        if (context.currentShader->GetType() == ShaderType::Fragment)
        {
            if (context.currentShader->FindInput(m_Name))
            {
                return true;
            }
        }

        return false;
    }

    bool VariableReferenceNode::IsFragmentOutput(const ShaderCompileContext& context) const
    {
        if (context.currentShader->GetType() == ShaderType::Fragment)
        {
            if (context.currentShader->FindOutput(m_Name))
            {
                return true;
            }
        }

        return false;
    }

    void VariableReferenceNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        if (context.currentShader->GetUniformVariables().contains(m_Name))
        {
            outShader.append("inUniforms.");
        }
        else if (IsVertexOutput(context) || IsFragmentOutput(context))
        {
            outShader.append("out.");
        }
        else if (IsFragmentInput(context))
        {
            outShader.append("in.");
        }

        if (m_Index >= 0)
        {
            auto alloc = outShader.get_allocator();
            outShader += m_Name;
            outShader += string::ArenaFormat("{}", alloc, m_Index);
        }
        else if (!m_IndexVar.empty())
        {
            std::string varName = m_IndexVar;
            if (context.tempRenames.contains(varName))
            {
                varName = context.tempRenames.at(m_IndexVar);
            }

            auto alloc = outShader.get_allocator();
            outShader += m_Name;
            if (context.currentShader->FindInput(m_Name) || context.currentShader->FindOutput(m_Name))
            {
                outShader += string::ArenaFormat("{}", alloc, varName);
            }
            else
            {
                outShader += string::ArenaFormat("[{}]", alloc, varName);
            }
        }
        else
        {
            outShader += m_Name;

            if (IsVertexInput(context))
            {
                outShader += "[vertexId]";
            }
        }
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

        if (newNames.contains(m_IndexVar))
        {
            m_IndexVar = newNames.at(m_IndexVar);
        }
    }
}
