#include "ShaderStage.h"

#include "ASTNode.h"
#include "InputAttributeNode.h"
#include "OutputNode.h"
#include "engine/logging/Log.h"

namespace se::shader::ast
{
    void ShaderStage::AddInput(InputAttributeNode* node)
    {
        if (SPARK_VERIFY(!m_InputAttributes.contains(node->GetName())))
        {
            m_InputAttributes[node->GetName()] = node;
        }
    }

    void ShaderStage::AddOutput(OutputNode* node)
    {
        if (SPARK_VERIFY(!m_Outputs.contains(node->GetName())))
        {
            m_Outputs[node->GetName()] = node;
        }
    }

    void ShaderStage::AddNode(ASTNode* node)
    {
        if (!m_CurrentScope.empty())
        {
            m_CurrentScope.back().m_Node->Children.push_back(node);
        }
        else
        {
            m_AstNodes.push_back(node);
        }
    }

    void ShaderStage::PushScope(ASTNode* node)
    {
        m_CurrentScope.push_back(AstScope(node));
    }

    void ShaderStage::PopScope()
    {
        if (SPARK_VERIFY(!m_CurrentScope.empty()))
        {
            m_CurrentScope.pop_back();
        }
    }

    ASTNode* ShaderStage::FindInput(const std::string& name) const
    {
        //todo current scope

        if (m_InputAttributes.contains(name))
        {
            return m_InputAttributes.at(name);
        }

        return nullptr;
    }

    ASTNode* ShaderStage::FindOutput(const std::string& name) const
    {
        if (m_Outputs.contains(name))
        {
            return m_Outputs.at(name);
        }

        return nullptr;
    }
}
