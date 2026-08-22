module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:ElseNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    export class ElseNode : public ASTNode
    {
    SPARK_CLASS()

        ElseNode() = default;
        ElseNode(const ElseNode& rhs);

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
