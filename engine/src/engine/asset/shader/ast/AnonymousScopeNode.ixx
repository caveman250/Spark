module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:AnonymousScopeNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    class AnonymousScopeNode : public ASTNode
    {
        SPARK_CLASS()
        
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString &outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
