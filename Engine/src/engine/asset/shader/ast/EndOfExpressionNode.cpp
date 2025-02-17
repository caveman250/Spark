#include "EndOfExpressionNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(EndOfExpressionNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END(EndOfExpressionNode)

    std::string EndOfExpressionNode::GetDebugString() const
    {
        return "EndOfExpressionNode";
    }

    void EndOfExpressionNode::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }

    void EndOfExpressionNode::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }
}
