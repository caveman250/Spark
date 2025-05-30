#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class ElseNode : public ASTNode
    {
    SPARK_CLASS()

        ElseNode() = default;
        ElseNode(const ElseNode& rhs);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
