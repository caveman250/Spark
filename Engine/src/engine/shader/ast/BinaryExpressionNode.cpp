#include "BinaryExpressionNode.h"

namespace se::shader::ast
{
    BinaryExpressionNode::BinaryExpressionNode(OperatorType opType)
        : m_OpType(opType)
    {

    }

    std::string BinaryExpressionNode::GetDebugString() const
    {
        return std::format("BinaryExpressionNode - children {}", m_Children.size());
    }

    void BinaryExpressionNode::ToGlsl(string::ArenaString& outShader) const
    {
        SPARK_ASSERT(m_Children.size() == 2 || (m_Children.size() == 1 && m_OpType == OperatorType::Subtract), "Expected 1 or 2 children. Got {}", m_Children.size());

        auto alloc = outShader.get_allocator();

        if (m_Children.size() == 1)
        {
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[0]->ToGlsl(outShader);
        }
        else
        {
            m_Children[0]->ToGlsl(outShader);
            outShader.append(OperatorUtil::OperatorTypeToGlsl(m_OpType));
            m_Children[1]->ToGlsl(outShader);
        }
    }
}
