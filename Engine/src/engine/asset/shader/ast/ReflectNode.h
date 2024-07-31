#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class ReflectNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(ReflectNode)

        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;
    };
}
