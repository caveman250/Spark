#include "OutputNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(OutputNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_Var)
        DEFINE_SERIALIZED_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    OutputNode::OutputNode(const Variable& var, const std::string& name)
        : m_Var(var)
        , m_Name(name)
    {
    }

    std::string OutputNode::GetDebugString() const
    {
        return std::format("OutputNode - {}, {}", TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void OutputNode::ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const
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
        outShader += string::ArenaFormat("out {} {}{};\n", alloc, TypeUtil::TypeToGlsl(m_Var.type), m_Name, arrayTag);
    }

    void OutputNode::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        std::string arrayTag = "";
        if (!m_Var.arraySizeVariable.empty())
        {
            SPARK_ASSERT(false, "variables sized array not spported in shader outputs in mtl.");
        }

        if (m_Var.arraySizeConstant > 0)
        {
            for (size_t i = 0; i < m_Var.arraySizeConstant; ++i)
            {
                outShader += string::ArenaFormat("{} {}{};\n", alloc, TypeUtil::TypeToMtl(m_Var.type), m_Name, i);
            }
        }
        else
        {
            outShader += string::ArenaFormat("{} {};\n", alloc, TypeUtil::TypeToMtl(m_Var.type), m_Name);
        }
    }

    const Variable& OutputNode::GetVar() const
    {
        return m_Var;
    }

    const std::string& OutputNode::GetName() const
    {
        return m_Name;
    }
}
