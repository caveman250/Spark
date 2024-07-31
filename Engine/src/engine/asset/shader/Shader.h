#pragma once
#include "ast/InputPortNode.h"
#include "spark.h"
#include "ast/Types.h"
#include "engine/reflect/Reflect.h"

namespace se::asset::shader::compiler
{
    class Parser;
}

namespace se::asset::shader::ast
{
    class MainNode;
    class InputNode;
    class OutputNode;
    class InputPortNode;
    class OutputPortNode;
    class ASTNode;

    class AstScope
    {
    public:
        AstScope() {}
        AstScope(const std::shared_ptr<ASTNode>& node) : m_Node(node) {}
        std::shared_ptr<ASTNode> m_Node;

        std::map<std::string, AstType::Type> m_Variables;
    };

    class Shader : public reflect::ObjectBase
    {
        DECLARE_SPARK_CLASS(Shader)

        bool FindVariable(const std::string& name, AstType::Type& type) const;
        const std::map<std::string, std::shared_ptr<InputNode>>& GetInputs() const { return m_Inputs; }
        const std::map<std::string, std::shared_ptr<OutputNode>>& GetOutputs() const { return m_Outputs; }
        const std::map<std::string, std::shared_ptr<InputPortNode>>& GetInputPorts() const { return m_InputPorts; }
        const std::map<std::string, std::shared_ptr<OutputPortNode>>& GetOutputPorts() const { return m_OutputPorts; }
        const std::vector<std::shared_ptr<ASTNode>>& GetNodes() const { return m_AstNodes; }
        std::map<std::string, AstType::Type>& GetGlobalVariables() { return m_GlobalVariables; }
        const std::map<std::string, AstType::Type>& GetUniformVariables() const { return m_Uniforms; }
        std::vector<AstScope>& GetScopeStack() { return m_ScopeStack; }

        void AddInputPort(const std::shared_ptr<InputPortNode>& node);
        void AddOutputPort(const std::shared_ptr<OutputPortNode>& node);
        void RemoveInputPort(const std::string& varName);
        void RemoveOutputPort(const std::string& varName);
        void AddInput(const std::shared_ptr<InputNode>& node);
        void AddOutput(const std::shared_ptr<OutputNode>& node);
        void AddNode(const std::shared_ptr<ASTNode>& node);
        void PushScope(const std::shared_ptr<ASTNode>& node);
        void PopScope();
        uint8_t ScopeDepth() { return static_cast<uint8_t>(m_ScopeStack.size()); }
        bool IsMainDeclared() { return m_MainDeclared; }
        bool IsMainCurrentScope();

        std::pair<uint32_t, std::shared_ptr<MainNode>> FindMain() const;

        const std::shared_ptr<InputPortNode>& FindInputPort(const std::string& name) const;
        const std::shared_ptr<InputPortNode>& FindInputPortByPortName(const std::string& portName) const;
        const std::shared_ptr<OutputPortNode>& FindOutputPort(const std::string& name) const;
        const std::shared_ptr<OutputPortNode>& FindOutputPortByPortName(const std::string& portName) const;
        const std::shared_ptr<InputNode>& FindInput(const std::string& name) const;
        const std::shared_ptr<OutputNode>& FindOutput(const std::string& name) const;
        bool RecordVariableForScope(const std::string& name, const AstType::Type& type, std::string& outError);
        bool AddUniform(const std::string& name, const AstType::Type& type, std::string& outError);

        void InsertNode(size_t at, const std::shared_ptr<ASTNode>& node);
        bool HasUniform(const std::string& name, AstType::Type type);

    private:
        bool m_MainDeclared = false;
        std::map<std::string, std::shared_ptr<InputPortNode>> m_InputPorts;
        std::map<std::string, std::shared_ptr<OutputPortNode>> m_OutputPorts;
        std::map<std::string, std::shared_ptr<InputNode>> m_Inputs;
        std::map<std::string, std::shared_ptr<OutputNode>> m_Outputs;
        std::vector<std::shared_ptr<ASTNode>> m_AstNodes;
        std::vector<AstScope> m_ScopeStack;
        std::map<std::string, AstType::Type> m_Uniforms;
        std::map<std::string, AstType::Type> m_GlobalVariables;
    };
}
