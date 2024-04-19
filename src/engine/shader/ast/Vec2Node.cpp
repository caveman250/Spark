#include "Vec2Node.h"

namespace se::shader::ast
{
    void Vec2Node::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("vec2(");
        for (int i = 0; i < m_Children.size(); ++i)
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
