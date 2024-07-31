#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Vec2Node : public ASTNode
    {
        DECLARE_SPARK_CLASS(Vec2Node)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
