#include "Vec2Node.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(Vec2Node)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string Vec2Node::GetDebugString() const
    {
        return "Vec2Node";
    }

    void Vec2Node::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("vec2(");
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
