module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:PropertyAccessNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;

    export class PropertyAccessNode : public ASTNode
    {
        SPARK_CLASS()

    public:
        PropertyAccessNode() {}
        PropertyAccessNode(const std::string& propName);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    private:
        SPARK_MEMBER(Serialized)
        std::string m_PropertyName = {};
    };
}
