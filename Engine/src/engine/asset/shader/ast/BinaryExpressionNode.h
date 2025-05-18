#pragma once

#include "ASTNode.h"
#include "Operators.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class BinaryExpressionNode : public ASTNode
    {
        SPARK_CLASS(BinaryExpressionNode)

    public:
        BinaryExpressionNode() {}
        explicit BinaryExpressionNode(OperatorType::Type opType);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        OperatorType::Type GetOperatorType() const { return m_OpType; }

    private:
        SPARK_MEMBER(Serialized)
        OperatorType::Type m_OpType;
    };
}
