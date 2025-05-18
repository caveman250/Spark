#include "BinaryExpressionNode.h"

namespace se::asset::shader::ast
{
    BinaryExpressionNode::BinaryExpressionNode(OperatorType::Type opType)
        : m_OpType(opType)
    {

    }

    std::string BinaryExpressionNode::GetDebugString() const
    {
        return std::format("BinaryExpressionNode - {}, children {}", OperatorUtil::OperatorTypeToGlsl(m_OpType), m_Children.size());
    }

    void BinaryExpressionNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        SPARK_ASSERT(m_Children.size() == 2 || (m_Children.size() == 1 && m_OpType == OperatorType::Subtract), "Expected 1 or 2 children. Got {}", m_Children.size());

        auto alloc = outShader.get_allocator();

        if (m_Children.size() == 1)
        {
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[0]->ToGlsl(context, outShader);
        }
        else
        {
            m_Children[0]->ToGlsl(context, outShader);
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[1]->ToGlsl(context, outShader);
        }
    }

    void BinaryExpressionNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        SPARK_ASSERT(m_Children.size() == 2 || (m_Children.size() == 1 && m_OpType == OperatorType::Subtract), "Expected 1 or 2 children. Got {}", m_Children.size());

        auto alloc = outShader.get_allocator();

        if (m_Children.size() == 1)
        {
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[0]->ToMtl(context, outShader);
        }
        else
        {
            m_Children[0]->ToMtl(context, outShader);
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[1]->ToMtl(context, outShader);
        }
    }
}
