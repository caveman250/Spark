#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Vec4Node : public ASTNode
    {
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
