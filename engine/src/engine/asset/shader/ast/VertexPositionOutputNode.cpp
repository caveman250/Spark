#include "VertexPositionOutputNode.h"

#include "ShaderCompileContext.h"

namespace se::asset::shader::ast
{
    std::string VertexPositionOutputNode::GetDebugString() const
    {
        return "VertexPositionOutputNode";
    }

    void VertexPositionOutputNode::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += "gl_Position";
    }

    void VertexPositionOutputNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader.append("out.");
        outShader.append(context.vertexPositionOutputName.Data());
    }
}
