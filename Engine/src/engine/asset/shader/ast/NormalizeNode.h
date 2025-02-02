#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class NormalizeNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(NormalizeNode)

        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
