#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::shader::ast
{
    class LengthNode : public ASTNode
    {
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}