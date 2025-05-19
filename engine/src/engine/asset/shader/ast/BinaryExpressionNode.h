#pragma once

#include "ASTNode.h"
#include "Operators.h"
#include "spark.h"

namespace se::asset::shader::ast
{
    class BinaryExpressionNode : public ASTNode
    {
        SPARK_CLASS()

    public:
        BinaryExpressionNode() {}
        explicit BinaryExpressionNode(OperatorType opType);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;

        OperatorType GetOperatorType() const { return m_OpType; }

    private:
        SPARK_MEMBER(Serialized)
        OperatorType m_OpType;
    };
}
