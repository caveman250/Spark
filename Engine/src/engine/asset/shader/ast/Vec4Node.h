#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Vec4Node : public ASTNode
    {
        DECLARE_SPARK_CLASS(Vec4Node)

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
