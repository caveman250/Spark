#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Vec3Node : public ASTNode
    {
        DECLARE_SPARK_CLASS(Vec3Node)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
