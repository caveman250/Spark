#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class MainNode : public ASTNode
    {
    public:
        DECLARE_SPARK_CLASS(MainNode)

        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
