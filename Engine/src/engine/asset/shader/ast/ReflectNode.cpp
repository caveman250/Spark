#include "ReflectNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(ReflectNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string ReflectNode::GetDebugString() const
    {
        return "ReflectNode";
    }

    void ReflectNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("reflect(");
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
