#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class Vec2Node : public ASTNode
    {
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
