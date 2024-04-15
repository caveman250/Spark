#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class MainNode : public ASTNode
    {
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
