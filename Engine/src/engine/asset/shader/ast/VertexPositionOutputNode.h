#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class VertexPositionOutputNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(VertexPositionOutputNode)

        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
