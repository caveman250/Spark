#include "ShaderStage.h"

#include <set>

#include "AssignmentNode.h"
#include "ASTNode.h"
#include "EndOfExpressionNode.h"
#include "InputAttributeNode.h"
#include "InputPortNode.h"
#include "OutputPortNode.h"
#include "MainNode.h"
#include "NameGenerator.h"
#include "OutputNode.h"
#include "VariableDeclarationNode.h"
#include "VariableReferenceNode.h"
#include "engine/logging/Log.h"

namespace se::shader::ast
{
    void ShaderStage::AddInputPort(InputPortNode *node)
    {
        if (SPARK_VERIFY(FindInputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_InputPorts[node->GetName()] = node;
        }
    }

    void ShaderStage::AddOutputPort(OutputPortNode *node)
    {
        if (SPARK_VERIFY(FindOutputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_OutputPorts[node->GetName()] = node;
        }
    }

    void ShaderStage::RemoveInputPort(const std::string &varName)
    {
        m_InputPorts.erase(varName);
    }

    void ShaderStage::RemoveOutputPort(const std::string &varName)
    {
        m_OutputPorts.erase(varName);
    }

    void ShaderStage::AddInput(InputAttributeNode *node)
    {
        if (SPARK_VERIFY(!m_InputAttributes.contains(node->GetName())))
        {
            m_InputAttributes[node->GetName()] = node;
        }
    }

    void ShaderStage::AddOutput(OutputNode *node)
    {
        if (SPARK_VERIFY(!m_Outputs.contains(node->GetName())))
        {
            m_Outputs[node->GetName()] = node;
        }
    }

    void ShaderStage::AddNode(ASTNode *node)
    {
        if (dynamic_cast<MainNode *>(node))
        {
            if (!SPARK_VERIFY(m_ScopeStack.empty()))
            {
                logging::Log::Error("ShaderStage::AddNode - Main declared within another scope.");
            }

            m_MainDeclared = true;
        }

        if (!m_ScopeStack.empty())
        {
            m_ScopeStack.back().m_Node->m_Children.push_back(node);
        }
        else
        {
            m_AstNodes.push_back(node);
        }
    }

    void ShaderStage::PushScope(ASTNode *node)
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
        return m_ScopeStack.size() == 1 && dynamic_cast<MainNode *>(m_ScopeStack[0].m_Node) != nullptr;
    }

    bool ShaderStage::FindVariable(const std::string &name, Type &type) const
    {
        for (int i = m_ScopeStack.size() - 1; i > -1; --i)
        {
            if (m_ScopeStack[i].m_Variables.contains(name))
            {
                type = m_ScopeStack[i].m_Variables.at(name);
                return true;
            }
        }

        if (m_GlobalVariables.contains(name))
        {
            type = m_GlobalVariables.at(name);
            return true;
        }

        if (m_Uniforms.contains(name))
        {
            type = m_Uniforms.at(name);
            return true;
        }

        if (InputPortNode *input = FindInputPort(name))
        {
            type = input->GetType();
            return true;
        }

        if (OutputPortNode *output = FindOutputPort(name))
        {
            type = output->GetType();
            return true;
        }

        if (InputAttributeNode *input = FindInputAttribute(name))
        {
            type = input->GetType();
            return true;
        }

        if (OutputNode *output = FindOutput(name))
        {
            type = output->GetType();
            return true;
        }

        return false;
    }

    InputPortNode *ShaderStage::FindInputPort(const std::string &name) const
    {
        if (m_InputPorts.contains(name))
        {
            return m_InputPorts.at(name);
        }

        return nullptr;
    }

    InputPortNode * ShaderStage::FindInputPortByPortName(const std::string &portName) const
    {
        for (const auto &[varName, port]: m_InputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    OutputPortNode *ShaderStage::FindOutputPort(const std::string &name) const
    {
        if (m_OutputPorts.contains(name))
        {
            return m_OutputPorts.at(name);
        }

        return nullptr;
    }

    OutputPortNode *ShaderStage::FindOutputPortByPortName(const std::string &portName) const
    {
        for (const auto &[varName, port]: m_OutputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }
        return nullptr;
    }

    InputAttributeNode *ShaderStage::FindInputAttribute(const std::string &name) const
    {
        if (m_InputAttributes.contains(name))
        {
            return m_InputAttributes.at(name);
        }

        return nullptr;
    }

    OutputNode *ShaderStage::FindOutput(const std::string &name) const
    {
        if (m_Outputs.contains(name))
        {
            return m_Outputs.at(name);
        }

        return nullptr;
    }

    bool ShaderStage::RecordVariableForScope(const std::string &name, const Type &type, std::string &outError)
    {
        if (m_GlobalVariables.contains(name))
        {
            outError = std::format("Varible {0} has already been declared!", name);
            return false;
        }

        if (m_ScopeStack.empty())
        {
            m_GlobalVariables[name] = type;
            return true;
        }

        for (auto it = m_ScopeStack.rbegin(); it != m_ScopeStack.rend(); ++it)
        {
            if (it->m_Variables.contains(name))
            {
                outError = std::format("Varible {0} has already been declared!", name);
                return false;
            }
        }

        m_ScopeStack.back().m_Variables[name] = type;
        return true;
    }

    bool ShaderStage::AddUniform(const std::string& name, const Type& type, std::string& outError)
    {
        if (m_Uniforms.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        m_Uniforms[name] = type;
    }

    void ShaderStage::InsertNode(size_t at, ASTNode *node)
    {
        m_AstNodes.insert(m_AstNodes.begin() + at, node);
    }

    bool ShaderStage::HasUniform(const std::string& name, Type type)
    {
        for (const auto& [uniformName, uniformType] : m_Uniforms)
        {
            if (name == uniformName && type == uniformType)
            {
                return true;
            }
        }

        return false;
    }

    std::pair<uint32_t, MainNode *> ShaderStage::FindMain() const
    {
        for (size_t i = 0; i < m_AstNodes.size(); ++i)
        {
            if (auto main = dynamic_cast<MainNode *>(m_AstNodes[i]))
            {
                return {static_cast<uint32_t>(i), main};
            }
        }

        return {-1, nullptr};
    }
}
