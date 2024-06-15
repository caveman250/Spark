#include "Vec4Node.h"

namespace se::shader::ast
{
    void Vec4Node::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("vec4(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            auto* child = m_Children[i];
            child->ToGlsl(outShader);
            if (i < m_Children.size() - 1)
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
