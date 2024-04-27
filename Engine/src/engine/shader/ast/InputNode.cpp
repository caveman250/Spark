#include "InputNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"

namespace se::shader::ast
{
    InputNode::InputNode(Type type, const std::string& name)
        : m_Type(type)
        , m_Name(name)
    {
    }

    void InputNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("in {} {};\n", alloc, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    Type InputNode::GetType() const
    {
        return m_Type;
    }

    const std::string& InputNode::GetName() const
    {
        return m_Name;
    }
}
