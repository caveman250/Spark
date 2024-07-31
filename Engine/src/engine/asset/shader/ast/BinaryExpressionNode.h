#pragma once

#include "ASTNode.h"
#include "Operators.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class BinaryExpressionNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(BinaryExpressionNode)

    public:
        BinaryExpressionNode() {}
        explicit BinaryExpressionNode(OperatorType::Type opType);
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

    private:
        OperatorType::Type m_OpType;
    };
}
