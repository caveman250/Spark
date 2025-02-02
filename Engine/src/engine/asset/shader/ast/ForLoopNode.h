#pragma once

#include "ASTNode.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class VariableDeclarationNode;
}

namespace se::asset::shader::ast
{
    class ForLoopNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(ForLoopNode)

        ForLoopNode() = default;
        ForLoopNode(const ForLoopNode& rhs);
        void ApplyNameRemapping(const std::map<std::string, std::string>& newNames) override;

        const std::shared_ptr<ASTNode>& AddChild(ASTNode* node) override;
        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;

        std::vector<std::shared_ptr<ASTNode>> m_Declaration;
        std::vector<std::shared_ptr<ASTNode>> m_Condition;
        std::vector<std::shared_ptr<ASTNode>> m_Expression;

        //Used be parser. Not serialised.
        bool m_DeclarationEnded = false;
        bool m_ConditionEnded = false;
        bool m_ExpressionEnded = false;
    };
}
