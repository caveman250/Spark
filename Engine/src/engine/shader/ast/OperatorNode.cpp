#include "OperatorNode.h"

namespace se::shader::ast
{
    OperatorNode::OperatorNode(OperatorType type)
        : m_Type(type)
    {

    }

    void OperatorNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(string::ArenaFormat(" {} ", alloc, OperatorUtil::OperatorTypeToGlsl(m_Type)));
    }
}
