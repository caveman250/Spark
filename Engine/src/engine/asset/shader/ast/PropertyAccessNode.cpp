#include "PropertyAccessNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(PropertyAccessNode)
        DEFINE_MEMBER(m_Children)
        DEFINE_MEMBER(m_PropertyName)
    DEFINE_SPARK_CLASS_END()

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


