#pragma once

#include "ASTNode.h"
#include "Operators.h"
#include "spark.h"

namespace se::shader::ast
{
    class BinaryExpressionNode : public ASTNode
    {
    public:
        explicit BinaryExpressionNode(OperatorType opType);
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

    private:
        OperatorType m_OpType;
    };
}
