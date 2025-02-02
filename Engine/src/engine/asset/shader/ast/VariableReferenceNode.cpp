#include "VariableReferenceNode.h"

#include "InputAttributeNode.h"
#include "NameGenerator.h"
#include "OutputNode.h"
#include "ShaderCompileContext.h"
#include "../Shader.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(VariableReferenceNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_Type)
        DEFINE_SERIALIZED_MEMBER(m_Name)
        DEFINE_SERIALIZED_MEMBER(m_Index)
        DEFINE_SERIALIZED_MEMBER(m_IndexVar)
    DEFINE_SPARK_CLASS_END()

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

    void VariableReferenceNode::ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const
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

    void VariableReferenceNode::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        if (context.shader.GetUniformVariables().contains(m_Name))
        {
            outShader.append("inUniforms.");
        }

        if (m_Index >= 0)
        {
            auto alloc = outShader.get_allocator();
            outShader += m_Name;
            outShader += string::ArenaFormat("[{}]", alloc, m_Index);
        }
        else if (!m_IndexVar.empty())
        {
            auto alloc = outShader.get_allocator();
            outShader += m_Name;
            outShader += string::ArenaFormat("[{}]", alloc, m_IndexVar);
        }
        else if (context.shader.GetType() == ShaderType::Vertex)
        {
            if (context.shader.FindInput(m_Name))
            {
                auto alloc = outShader.get_allocator();
                outShader += m_Name;
                outShader += string::ArenaFormat("[vertexId]", alloc, m_Index);
            }
            else if (context.shader.FindOutput(m_Name) || context.vertexPositionOutputName == m_Name)
            {
                outShader += "out.";
                outShader += m_Name;
            }
            else
            {
                outShader += m_Name;
            }
        }
        else if (context.shader.GetType() == ShaderType::Fragment)
        {
            if (context.shader.FindInput(m_Name))
            {
                outShader += "in.";
                outShader += m_Name;
            }
            else
            {
                outShader += m_Name;
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
