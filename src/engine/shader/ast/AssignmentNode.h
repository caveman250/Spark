#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class AssignmentNode : public ASTNode
    {
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
