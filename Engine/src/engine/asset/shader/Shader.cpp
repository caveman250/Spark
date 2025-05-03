#include "Shader.h"

#include "ast/ASTNode.h"
#include "ast/InputNode.h"
#include "ast/InputPortNode.h"
#include "ast/OutputPortNode.h"
#include "ast/MainNode.h"
#include "ast/OutputNode.h"

DEFINE_SPARK_ENUM_BEGIN(se::asset::ShaderType)
    DEFINE_ENUM_VALUE(ShaderType, Vertex)
    DEFINE_ENUM_VALUE(ShaderType, Fragment)
DEFINE_SPARK_ENUM_END()

namespace se::asset
{
    DEFINE_SPARK_CLASS_BEGIN(Shader)
        DEFINE_SERIALIZED_MEMBER(m_Type)
        DEFINE_SERIALIZED_MEMBER(m_MainDeclared)
        DEFINE_SERIALIZED_MEMBER(m_InputPorts)
        DEFINE_SERIALIZED_MEMBER(m_OutputPorts)
        DEFINE_SERIALIZED_MEMBER(m_Inputs)
        DEFINE_SERIALIZED_MEMBER(m_Outputs)
        DEFINE_SERIALIZED_MEMBER(m_AstNodes)
        DEFINE_SERIALIZED_MEMBER(m_Uniforms)
        DEFINE_SERIALIZED_MEMBER(m_GlobalVariables)
        DEFINE_SERIALIZED_MEMBER(m_Settings)
    DEFINE_SPARK_CLASS_END(Shader)

    void Shader::AddInputPort(const std::shared_ptr<shader::ast::InputPortNode>& node)
    {
        if (SPARK_VERIFY(FindInputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_InputPorts[node->GetName()] = std::make_shared<shader::ast::InputPortNode>(*node);
        }
    }

    void Shader::AddOutputPort(const std::shared_ptr<shader::ast::OutputPortNode>& node)
    {
        if (SPARK_VERIFY(FindOutputPortByPortName(node->GetPortName()) == nullptr))
        {
            m_OutputPorts[node->GetName()] = std::make_shared<shader::ast::OutputPortNode>(*node);
        }
    }

    void Shader::RemoveInputPort(const String &varName)
    {
        m_InputPorts.erase(varName);
    }

    void Shader::RemoveOutputPort(const String &varName)
    {
        m_OutputPorts.erase(varName);
    }

    void Shader::AddInput(const std::shared_ptr<shader::ast::InputNode>& node)
    {
        if (SPARK_VERIFY(!m_Inputs.contains(node->GetName())))
        {
            auto newVal = std::shared_ptr<shader::ast::InputNode>((shader::ast::InputNode*)node->GetReflectType()->heap_copy_constructor(node.get()));
            m_Inputs.insert(std::make_pair(node->GetName(), newVal));
        }
    }

    void Shader::AddOutput(const std::shared_ptr<shader::ast::OutputNode>& node)
    {
        if (SPARK_VERIFY(!m_Outputs.contains(node->GetName())))
        {
            auto newVal = std::shared_ptr<shader::ast::OutputNode>((shader::ast::OutputNode*)node->GetReflectType()->heap_copy_constructor(node.get()));
            m_Outputs[node->GetName()] = newVal;
        }
    }

    std::shared_ptr<shader::ast::ASTNode> Shader::AddNode(shader::ast::ASTNode* node)
    {
        if (dynamic_cast<shader::ast::MainNode*>(node))
        {
            if (!SPARK_VERIFY(m_ScopeStack.empty()))
            {
                debug::Log::Error("ShaderStage::AddNode - Main declared within another scope.");
            }

            m_MainDeclared = true;
        }

        if (!m_ScopeStack.empty())
        {
            return m_ScopeStack.back().m_Node->AddChild((shader::ast::ASTNode*)node->GetReflectType()->heap_copy_constructor(node));
        }
        else
        {
            m_AstNodes.push_back(std::shared_ptr<shader::ast::ASTNode>((shader::ast::ASTNode*)node->GetReflectType()->heap_copy_constructor(node)));
            return m_AstNodes.back();
        }
    }

    void Shader::PushScope(const std::shared_ptr<shader::ast::ASTNode>& node)
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
        return m_ScopeStack.size() == 1 && std::dynamic_pointer_cast<shader::ast::MainNode>(m_ScopeStack[0].m_Node) != nullptr;
    }

    Shader::Shader(const Shader& rhs)
    {
        *this = rhs;
    }

    Shader& Shader::operator=(const Shader& rhs)
    {
        m_Type = rhs.m_Type;
        m_MainDeclared = rhs.m_MainDeclared;
        m_InputPorts.clear();
        for (const auto& [key, value] : rhs.m_InputPorts)
        {
            m_InputPorts[key] = std::make_shared<shader::ast::InputPortNode>(*value);
        }

        m_OutputPorts.clear();
        for (const auto& [key, value] : rhs.m_OutputPorts)
        {
            m_OutputPorts[key] = std::make_shared<shader::ast::OutputPortNode>(*value);
        }

        m_Inputs.clear();
        for (const auto& [key, value] : rhs.m_Inputs)
        {
            m_Inputs[key] = std::make_shared<shader::ast::InputNode>(*value);
        }

        m_Outputs.clear();
        for (const auto& [key, value] : rhs.m_Outputs)
        {
            m_Outputs[key] = std::make_shared<shader::ast::OutputNode>(*value);
        }

        m_AstNodes.clear();
        m_AstNodes.reserve(rhs.m_AstNodes.size());
        for (const auto& node : rhs.m_AstNodes)
        {
            auto* objBase = static_cast<ObjectBase*>(node.get());
            if (SPARK_VERIFY(objBase))
            {
                m_AstNodes.push_back(std::shared_ptr<shader::ast::ASTNode>((shader::ast::ASTNode*)objBase->GetReflectType()->heap_copy_constructor(node.get())));
            }
        }

        m_ScopeStack = rhs.m_ScopeStack; // this is busted. TODO move scope stack out of shader class.

        m_Uniforms = rhs.m_Uniforms;
        m_GlobalVariables = rhs.m_GlobalVariables;
        m_Settings = rhs.m_Settings;

        return *this;
    }

    bool Shader::FindVariable(const String &name, shader::ast::AstType::Type &type) const
    {
        for (int i = static_cast<int>(m_ScopeStack.size()) - 1; i > -1; --i)
        {
            if (m_ScopeStack[i].m_Variables.contains(name))
            {
                type = m_ScopeStack[i].m_Variables.at(name).type;
                return true;
            }
        }

        if (m_GlobalVariables.contains(name))
        {
            type = m_GlobalVariables.at(name).type;
            return true;
        }

        if (m_Settings.contains(name))
        {
            type = m_Settings.at(name).type;
            return true;
        }

        if (m_Uniforms.contains(name))
        {
            type = m_Uniforms.at(name).type;
            return true;
        }

        if (const std::shared_ptr<shader::ast::InputPortNode>& input = FindInputPort(name))
        {
            type = input->GetVar().type;
            return true;
        }

        if (const std::shared_ptr<shader::ast::OutputPortNode>& output = FindOutputPort(name))
        {
            type = output->GetVar().type;
            return true;
        }

        if (const std::shared_ptr<shader::ast::InputNode>& input = FindInput(name))
        {
            type = input->GetVar().type;
            return true;
        }

        if (const std::shared_ptr<shader::ast::OutputNode>& output = FindOutput(name))
        {
            type = output->GetVar().type;
            return true;
        }

        return false;
    }

    const std::shared_ptr<shader::ast::InputPortNode>& Shader::FindInputPort(const String &name) const
    {
        if (m_InputPorts.contains(name))
        {
            return m_InputPorts.at(name);
        }

        static std::shared_ptr<shader::ast::InputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<shader::ast::InputPortNode>&  Shader::FindInputPortByPortName(const String &portName) const
    {
        for (const auto &[varName, port]: m_InputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }

        static std::shared_ptr<shader::ast::InputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<shader::ast::OutputPortNode>& Shader::FindOutputPort(const String &name) const
    {
        if (m_OutputPorts.contains(name))
        {
            return m_OutputPorts.at(name);
        }

        static std::shared_ptr<shader::ast::OutputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<shader::ast::OutputPortNode>& Shader::FindOutputPortByPortName(const String &portName) const
    {
        for (const auto &[varName, port]: m_OutputPorts)
        {
            if (port->GetPortName() == portName)
            {
                return port;
            }
        }

        static std::shared_ptr<shader::ast::OutputPortNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<shader::ast::InputNode>& Shader::FindInput(const String &name) const
    {
        if (m_Inputs.contains(name))
        {
            return m_Inputs.at(name);
        }

        static std::shared_ptr<shader::ast::InputNode> nullRet = nullptr;
        return nullRet;
    }

    const std::shared_ptr<shader::ast::OutputNode>& Shader::FindOutput(const String &name) const
    {
        if (m_Outputs.contains(name))
        {
            return m_Outputs.at(name);
        }

        static std::shared_ptr<shader::ast::OutputNode> nullRet = nullptr;
        return nullRet;
    }

    bool Shader::RecordVariableForScope(const String &name, const shader::ast::Variable& var, String &outError)
    {
        if (m_GlobalVariables.contains(name))
        {
            outError = std::format("Varible {0} has already been declared!", name);
            return false;
        }

        if (m_Settings.contains(name))
        {
            outError = std::format("Varible {0} has already been declared!", name);
            return false;
        }

        if (m_Uniforms.contains(name))
        {
            outError = std::format("Varible {0} has already been declared!", name);
            return false;
        }

        if (m_ScopeStack.empty())
        {
            m_GlobalVariables[name] = var;
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

        m_ScopeStack.back().m_Variables[name] = var;
        return true;
    }

    bool Shader::AddUniform(const String& name, const shader::ast::Variable& var, String& outError)
    {
        if (m_GlobalVariables.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        if (m_Settings.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        if (m_Uniforms.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        m_Uniforms[name] = var;
        return true;
    }

    bool Shader::HasUniform(const String& name) const
    {
        if (m_Inputs.contains(name))
        {
            return true;
        }

        return false;
    }

    bool Shader::AddSetting(const String& name, const shader::ast::Variable& var, String& outError)
    {
        if (m_GlobalVariables.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        if (m_Settings.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        if (m_Uniforms.contains(name))
        {
            outError = std::format("{} redefinition", name);
            return false;
        }

        m_Settings[name] = var;
        return true;
    }

    void Shader::InsertNode(size_t at, const std::shared_ptr<shader::ast::ASTNode>& node)
    {
        m_AstNodes.insert(m_AstNodes.begin() + at, std::shared_ptr<shader::ast::ASTNode>((shader::ast::ASTNode*)node->GetReflectType()->heap_copy_constructor(node.get())));
    }

    bool Shader::HasUniform(const String& name, const shader::ast::Variable& var)
    {
        for (const auto& [uniformName, uniformVar] : m_Uniforms)
        {
            if (name == uniformName && var == uniformVar)
            {
                return true;
            }
        }

        return false;
    }

    bool Shader::HasSetting(const String& name, const shader::ast::Variable& var)
    {
        for (const auto& [settingName, settingVar] : m_Settings)
        {
            if (name == settingName && var == settingVar)
            {
                return true;
            }
        }

        return false;
    }

    std::pair<uint32_t, std::shared_ptr<shader::ast::MainNode>> Shader::FindMain() const
    {
        for (size_t i = 0; i < m_AstNodes.size(); ++i)
        {
            if (const auto& main = std::dynamic_pointer_cast<shader::ast::MainNode>(m_AstNodes[i]))
            {
                return {static_cast<uint32_t>(i), main};
            }
        }

        static std::shared_ptr<shader::ast::MainNode> nullRet = nullptr;
        return {-1, nullRet };
    }
}
