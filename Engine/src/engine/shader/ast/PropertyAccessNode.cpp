#include "PropertyAccessNode.h"

namespace se::shader::ast
{
    PropertyAccessNode::PropertyAccessNode(const std::string& propName)
        : m_PropertyName(propName)
    {
    }

    std::string PropertyAccessNode::GetDebugString() const
    {
        return std::format("PropertyAccessNode - {}", m_PropertyName);
    }

    void PropertyAccessNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        SPARK_ASSERT(m_Children.size() <= 1);
        if (!m_Children.empty())
        {
            m_Children[0]->ToGlsl(outShader);
        }
        outShader.append(string::ArenaFormat(".{}", alloc, m_PropertyName));
    }
}


