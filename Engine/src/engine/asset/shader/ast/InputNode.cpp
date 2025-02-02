#include "InputNode.h"
#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(InputNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
        DEFINE_SERIALIZED_MEMBER(m_Var)
        DEFINE_SERIALIZED_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    InputNode::InputNode(const Variable& var, const std::string& name)
        : m_Var(var)
        , m_Name(name)
    {
    }

    std::string InputNode::GetDebugString() const
    {
        return std::format("InputNode - {}, {}", TypeUtil::TypeToGlsl(m_Var.type), m_Name);
    }

    void InputNode::ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const
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
        outShader += string::ArenaFormat("in {} {}{};\n", alloc, TypeUtil::TypeToGlsl(m_Var.type), m_Name, arrayTag);
    }

    void InputNode::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
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
        outShader += string::ArenaFormat("{} {}{};\n", alloc, TypeUtil::TypeToMtl(m_Var.type), m_Name, arrayTag);
    }

    const Variable& InputNode::GetVar() const
    {
        return m_Var;
    }

    const std::string& InputNode::GetName() const
    {
        return m_Name;
    }
}
