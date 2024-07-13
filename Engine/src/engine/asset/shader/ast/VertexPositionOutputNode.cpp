#include "VertexPositionOutputNode.h"

namespace se::asset::shader::ast
{
    std::string VertexPositionOutputNode::GetDebugString() const
    {
        return "VertexPositionOutputNode";
    }

    void VertexPositionOutputNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += "gl_Position";
    }
}
