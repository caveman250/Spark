#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class ClampNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(ClampNode)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
