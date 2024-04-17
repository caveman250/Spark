#include "PortInfo.h"

#include "engine/shader/ast/InputAttributeNode.h"
#include "engine/shader/ast/OutputNode.h"
#include "engine/shader/ast/VariableReferenceNode.h"

namespace se::shader::parser
{
    ast::ASTNode* PortInfo::ResolvePort(const std::string &portName)
    {
        if (m_PortWriteStack.contains(portName))
        {
            return m_PortWriteStack.at(portName).back();
        }

        return nullptr;
    }

    void PortInfo::RecordPortWrite(const std::string &portName, ast::ASTNode *writtenTo)
    {
        m_PortWriteStack[portName].push_back(writtenTo);
    }

    std::string PortInfo::GetLastPortVariableName(const std::string& portName)
    {
        if (auto* node = ResolvePort(portName))
        {
            if (auto* variableRef = dynamic_cast<ast::VariableReferenceNode*>(node))
            {
                return variableRef->GetName();
            }
            else if (auto* inputAttrib = dynamic_cast<ast::InputAttributeNode*>(node))
            {
                return inputAttrib->GetName();
            }
            else if (auto* output = dynamic_cast<ast::OutputNode*>(node))
            {
                return output->GetName();
            }
            else
            {
                SPARK_ASSERT(false);
            }
        }

        return {};
    }
}
