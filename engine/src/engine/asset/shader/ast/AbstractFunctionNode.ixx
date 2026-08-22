module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:AbstractFunctionNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    export class AbstractFunctionNode : public ASTNode
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
