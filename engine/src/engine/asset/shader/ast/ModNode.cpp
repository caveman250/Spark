#include "ModNode.h"

namespace se::asset::shader::ast
{
    std::string ModNode::GetDebugString() const
    {
        return "ModNode";
    }

    void ModNode::ToGlsl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("mod(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            const auto& child = m_Children[i];
            child->ToGlsl(context, outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }

    void ModNode::ToMtl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        SPARK_ASSERT(m_Children.size() == 2);
        const auto* child1 = m_Children[0].get();
        const auto* child2 = m_Children[1].get();

        outShader.append("(");
        child1->ToMtl(context, outShader);
        outShader.append(") - (");
        child2->ToMtl(context, outShader);
        outShader.append(")");

        outShader.append(" * floor((");
        child1->ToMtl(context, outShader);
        outShader.append(") / (");
        child2->ToMtl(context, outShader);
        outShader.append("))");
    }
}
