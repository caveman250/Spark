#include "ClampNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(ClampNode)
        DEFINE_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string ClampNode::GetDebugString() const
    {
        return "ClampNode";
    }

    void ClampNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("clamp(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            const auto& child = m_Children[i];
            child->ToGlsl(outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
