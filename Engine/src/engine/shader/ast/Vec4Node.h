#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class Vec4Node : public ASTNode
    {
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
