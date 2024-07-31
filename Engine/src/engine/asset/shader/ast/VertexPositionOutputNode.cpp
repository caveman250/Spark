#include "VertexPositionOutputNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(VertexPositionOutputNode)
        DEFINE_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string VertexPositionOutputNode::GetDebugString() const
    {
        return "VertexPositionOutputNode";
    }

    void VertexPositionOutputNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += "gl_Position";
    }
}
