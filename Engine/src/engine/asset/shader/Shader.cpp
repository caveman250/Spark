#include "Shader.h"

#include <set>

#include "ast/ASTNode.h"
#include "ast/EndOfExpressionNode.h"
#include "ast/InputNode.h"
#include "ast/InputPortNode.h"
#include "ast/OutputPortNode.h"
#include "ast/MainNode.h"
#include "ast/NameGenerator.h"
#include "ast/OutputNode.h"
#include "ast/VariableDeclarationNode.h"
#include "ast/VariableReferenceNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(Shader)
        DEFINE_MEMBER(m_MainDeclared)
        DEFINE_MEMBER(m_InputPorts)
        DEFINE_MEMBER(m_OutputPorts)
        DEFINE_MEMBER(m_Inputs)
        DEFINE_MEMBER(m_Outputs)
        DEFINE_MEMBER(m_AstNodes)
        DEFINE_MEMBER(m_Uniforms)
        DEFINE_MEMBER(m_GlobalVariables)
    DEFINE_SPARK_CLASS_END()

    void Shader::AddInputPort(const std::shared_ptr<InputPortNode>& node)
    {
        if (SPARK_VERIFY(FindInputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_InputPorts[node->GetName()] = node;
        }
    }

    void Shader::AddOutputPort(const std::shared_ptr<OutputPortNode>& node)
    {
        if (SPARK_VERIFY(FindOutputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_OutputPorts[node->GetName()] = node;
        }
    }

    void Shader::RemoveInputPort(const std::string &varName)
    {
        m_InputPorts.erase(varName);
    }

    void Shader::RemoveOutputPort(const std::string &varName)
    {
        m_OutputPorts.erase(varName);
    }

    void Shader::AddInput(const std::shared_ptr<InputNode>& node)
    {
        if (SPARK_VERIFY(!m_Inputs.contains(node->GetName())))
        {
            m_Inputs[node->GetName()] = node;
        }
    }

    void Shader::AddOutput(const std::shared_ptr<OutputNode>& node)
    {
        if (SPARK_VERIFY(!m_Outputs.contains(node->GetName())))
        {
            m_Outputs[node->GetName()] = node;
        }
    }

    void Shader::AddNode(const std::shared_ptr<ASTNode>& node)
    {
        if (std::dynamic_pointer_cast<MainNode>(node))
        {
            if (!SPARK_VERIFY(m_ScopeStack.empty()))
            {
                debug::Log::Error("ShaderStage::AddNode - Main declared within another scope.");
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

    void Shader::PushScope(const std::shared_ptr<ASTNode>& node)
    {
        m_ScopeStack.push_back(AstScope(node));
    }

    void Shader::PopScope()
    {
        if (SPARK_VERIFY(!m_ScopeStack.empty()))
        {
            m_ScopeStack.pop_back();
        }
    }

    bool Shader::IsMainCurrentScope()
    {
        return m_ScopeStack.size() == 1 && std::dynamic_pointer_cast<MainNode>(m_ScopeStack[0].m_Node) != nullptr;
    }

    bool Shader::FindVariable(const std::string &name, AstType::Type &type) const
    {
        for (int i = static_cast<int>(m_ScopeStack.size()) - 1; i > -1; --i)
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

        if (const std::shared_ptr<InputPortNode>& input = FindInputPort(name))
        {
            type = input->GetType();
            return true;
        }

        if (const std::shared_ptr<OutputPortNode>& output = FindOutputPort(name))
        {
            type = output->GetType();
            return true;
        }

        if (const std::shared_ptr<InputNode>& input = FindInput(name))
        {
            type = input->GetType();
            return true;
        }

        if (const std::shared_ptr<OutputNode>& output = FindOutput(name))
        {
            type = output->GetType();
            return true;
        }

        return false;
    }

    const std::shared_ptr<InputPortNode>& Shader::FindInputPort(const std::string &name) const
    {
        if (m_InputPorts.contains(name))
        {
            return m_InputPorts.at(name);
        }

        static std::shared_ptr<InputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<InputPortNode>&  Shader::FindInputPortByPortName(const std::string &portName) const
    {
        for (const auto &[varName, port]: m_InputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }

        static std::shared_ptr<InputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<OutputPortNode>& Shader::FindOutputPort(const std::string &name) const
    {
        if (m_OutputPorts.contains(name))
        {
            return m_OutputPorts.at(name);
        }

        static std::shared_ptr<OutputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<OutputPortNode>& Shader::FindOutputPortByPortName(const std::string &portName) const
    {
        for (const auto &[varName, port]: m_OutputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }

        static std::shared_ptr<OutputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<InputNode>& Shader::FindInput(const std::string &name) const
    {
        if (m_Inputs.contains(name))
        {
            return m_Inputs.at(name);
        }

        static std::shared_ptr<InputNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<OutputNode>& Shader::FindOutput(const std::string &name) const
    {
        if (m_Outputs.contains(name))
        {
            return m_Outputs.at(name);
        }

        static std::shared_ptr<OutputNode> nullRet = nullptr;
        return nullRet;
    }

    bool Shader::RecordVariableForScope(const std::string &name, const AstType::Type &type, std::string &outError)
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

    bool Shader::AddUniform(const std::string& name, const AstType::Type& type, std::string& outError)
    {
        if (m_Uniforms.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        m_Uniforms[name] = type;
        return true;
    }

    void Shader::InsertNode(size_t at, const std::shared_ptr<ASTNode>& node)
    {
        m_AstNodes.insert(m_AstNodes.begin() + at, node);
    }

    bool Shader::HasUniform(const std::string& name, AstType::Type type)
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

    std::pair<uint32_t, std::shared_ptr<MainNode>> Shader::FindMain() const
    {
        for (size_t i = 0; i < m_AstNodes.size(); ++i)
        {
            if (const auto& main = std::dynamic_pointer_cast<MainNode>(m_AstNodes[i]))
            {
                return {static_cast<uint32_t>(i), main};
            }
        }

        static std::shared_ptr<MainNode> nullRet = nullptr;
        return {-1, nullRet };
    }
}
