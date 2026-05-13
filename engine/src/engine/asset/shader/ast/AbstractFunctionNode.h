#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class AbstractFunctionNode : public ASTNode
    {
        SPARK_CLASS()

        AbstractFunctionNode(const std::string& name, const std::string& glslName, const std::string& metalName);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        std::string m_Name;
        std::string m_GlslName;
        std::string m_MetalName;
    };
}