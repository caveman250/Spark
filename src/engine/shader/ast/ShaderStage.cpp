#include "ShaderStage.h"

#include "ASTNode.h"
#include "InputAttributeNode.h"
#include "MainNode.h"
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
        if (dynamic_cast<MainNode*>(node))
        {
            if (!SPARK_VERIFY(m_ScopeStack.empty()))
            {
                logging::Log::Error("ShaderStage::AddNode - Main declared within another scope.");
            }

            m_MainDeclared = true;
        }

        if (!m_ScopeStack.empty())
        {
            m_ScopeStack.back().m_Node->Children.push_back(node);
        }
        else
        {
            m_AstNodes.push_back(node);
        }
    }

    void ShaderStage::PushScope(ASTNode* node)
    {
        m_ScopeStack.push_back(AstScope(node));
    }

    void ShaderStage::PopScope()
    {
        if (SPARK_VERIFY(!m_ScopeStack.empty()))
        {
            m_ScopeStack.pop_back();
        }
    }

    bool ShaderStage::IsMainCurrentScope()
    {
        return m_ScopeStack.size() == 1 && dynamic_cast<MainNode*>(m_ScopeStack[0].m_Node) != nullptr;
    }

    bool ShaderStage::FindVariable(const std::string& name, Type& type) const
    {
        for (int i = m_ScopeStack.size() - 1; i > -1; --i)
        {
            if (m_ScopeStack[i].m_Variables.contains(name))
            {
                type = m_ScopeStack[i].m_Variables.at(name);
                return true;
            }
        }

        if (InputAttributeNode* input = FindInputAttribute(name))
        {
            type = input->GetType();
            return true;
        }

        if (OutputNode* output = FindOutput(name))
        {
            type = output->GetType();
            return true;
        }

        return false;
    }

    InputAttributeNode* ShaderStage::FindInputAttribute(const std::string& name) const
    {
        if (m_InputAttributes.contains(name))
        {
            return m_InputAttributes.at(name);
        }

        return nullptr;
    }

    OutputNode* ShaderStage::FindOutput(const std::string& name) const
    {
        if (m_Outputs.contains(name))
        {
            return m_Outputs.at(name);
        }

        return nullptr;
    }
}
