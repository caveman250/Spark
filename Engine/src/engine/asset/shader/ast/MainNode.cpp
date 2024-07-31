#include "MainNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(MainNode)
        DEFINE_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string MainNode::GetDebugString() const
    {
        return "MainNode";
    }

    void MainNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader.append("void main()\n{\n");
        for (const auto& child : m_Children)
        {
           child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
