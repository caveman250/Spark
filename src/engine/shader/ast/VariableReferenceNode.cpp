#include "VariableNode.h"

#include "NameGenerator.h"

namespace se::shader::ast
{
    VariableNode::VariableNode(Type type)
        : m_Type(type),
          m_Name(NameGenerator::GetName())
    {
    }

    std::string VariableNode::ToGlsl() const
    {
        return
    }
}
