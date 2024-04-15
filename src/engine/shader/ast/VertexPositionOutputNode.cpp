#include "VertexPositionOutputNode.h"

namespace se::shader::ast
{
    void VertexPositionOutputNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += "gl_Position";
    }
}
