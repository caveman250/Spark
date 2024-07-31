#include "OutputNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(OutputNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Type)
        DEFINE_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    OutputNode::OutputNode(AstType::Type type, const std::string& name)
        : m_Type(type)
        , m_Name(name)
    {
    }

    std::string OutputNode::GetDebugString() const
    {
        return std::format("OutputNode - {}, {}", TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void OutputNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("out {} {};\n", alloc, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    AstType::Type OutputNode::GetType() const
    {
        return m_Type;
    }

    const std::string& OutputNode::GetName() const
    {
        return m_Name;
    }
}
