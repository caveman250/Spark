#include "Vec4Node.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(Vec4Node)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string Vec4Node::GetDebugString() const
    {
        return "Vec4Node";
    }

    void Vec4Node::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("vec4(");
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
