module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:Vec3Node;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;

    class Vec3Node : public ASTNode
    {
        SPARK_CLASS()

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
