module;

#include <spark.h>
#include "engine/string/ArenaString.h"

export module Spark.Asset.Shader:BinaryExpressionNode;
import :ASTNode;

namespace se::asset::shader::ast
{
    struct ShaderCompileContext;
    enum class OperatorType;

    export class BinaryExpressionNode : public ASTNode
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
