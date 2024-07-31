#include "InputNode.h"
#include "TypeUtil.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(InputNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_Type)
        DEFINE_MEMBER(m_Name)
    DEFINE_SPARK_CLASS_END()

    InputNode::InputNode(AstType::Type type, const std::string& name)
        : m_Type(type)
        , m_Name(name)
    {
    }

    std::string InputNode::GetDebugString() const
    {
        return std::format("InputNode - {}, {}", TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    void InputNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("in {} {};\n", alloc, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    AstType::Type InputNode::GetType() const
    {
        return m_Type;
    }

    const std::string& InputNode::GetName() const
    {
        return m_Name;
    }
}
