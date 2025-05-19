#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class VariableDeclarationNode;
}

namespace se::asset::shader::ast
{
    class IfNode : public ASTNode
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
