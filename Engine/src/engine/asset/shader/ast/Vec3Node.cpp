#include "Vec3Node.h"

namespace se::asset::shader::ast
{
    std::string Vec3Node::GetDebugString() const
    {
        return "Vec3Node";
    }

    void Vec3Node::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("vec3(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            auto* child = m_Children[i];
            child->ToGlsl(outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
