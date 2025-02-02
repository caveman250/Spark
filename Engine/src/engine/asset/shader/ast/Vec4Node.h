#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Vec4Node : public ASTNode
    {
        DECLARE_SPARK_CLASS(Vec4Node)

        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
