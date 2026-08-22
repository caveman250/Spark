module;

#include <spark.h>
#include "engine/string/ArenaString.h"
#include <map>

export module Spark.Asset.Shader:IfNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    export class IfNode : public ASTNode
    {
        SPARK_CLASS()

        IfNode() = default;
        IfNode(const IfNode& rhs);
        void ApplyNameRemapping(const std::map<std::string, std::string>& newNames) override;
        void ForEachChild(const std::function<void(ASTNode*)>& func) override;

        const std::shared_ptr<ASTNode>& AddChild(ASTNode* node) override;
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        SPARK_MEMBER(Serialized)
        std::shared_ptr<ASTNode> m_Condition = nullptr;
    };
}
