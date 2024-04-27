#include "OutputNode.h"

#include "NameGenerator.h"
#include "TypeUtil.h"

namespace se::shader::ast
{
    OutputNode::OutputNode(Type type, const std::string& name)
        : m_Type(type)
        , m_Name(name)
    {
    }

    void OutputNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader += string::ArenaFormat("out {} {};\n", alloc, TypeUtil::GetTypeGlsl(m_Type), m_Name);
    }

    Type OutputNode::GetType() const
    {
        return m_Type;
    }

    const std::string& OutputNode::GetName() const
    {
        return m_Name;
    }
}
