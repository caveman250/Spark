#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class EndOfExpressionNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(EndOfExpressionNode)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
