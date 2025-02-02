#include "EndOfExpressionNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(EndOfExpressionNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string EndOfExpressionNode::GetDebugString() const
    {
        return "EndOfExpressionNode";
    }

    void EndOfExpressionNode::ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }

    void EndOfExpressionNode::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }
}
