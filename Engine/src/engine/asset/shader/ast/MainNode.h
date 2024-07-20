#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class MainNode : public ASTNode
    {
    public:
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
