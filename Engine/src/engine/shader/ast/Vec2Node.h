#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class Vec2Node : public ASTNode
    {
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
