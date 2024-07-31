#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class VertexPositionOutputNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(VertexPositionOutputNode)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
