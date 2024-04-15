#include "PropertyAccessNode.h"

namespace se::shader::ast
{
    PropertyAccessNode::PropertyAccessNode(const std::string& propName)
        : m_PropertyName(propName)
    {
    }

    void PropertyAccessNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat(".{}", alloc, m_PropertyName));
    }
}


