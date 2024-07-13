#pragma once
#include "ast/InputPortNode.h"
#include "spark.h"
#include "ast/Types.h"
#include "engine/asset/AssetBase.h"

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
        AstScope(ASTNode* node) : m_Node(node) {}
        ASTNode* m_Node;

        std::map<std::string, Type> m_Variables;
    };

    class Shader : public AssetBase
    {
    public:
        std::shared_ptr<binary::Database> Serialise() override;
        void Deserialise(const std::shared_ptr<binary::Database>& db) override;

    private:
        void AddInputPort(InputPortNode* node);
        void AddOutputPort(OutputPortNode* node);
        void RemoveInputPort(const std::string& varName);
        void RemoveOutputPort(const std::string& varName);
        void AddInput(InputNode* node);
        void AddOutput(OutputNode* node);
        void AddNode(ASTNode* node);
        void PushScope(ASTNode* node);
        void PopScope();
        uint8_t ScopeDepth() { return static_cast<uint8_t>(m_ScopeStack.size()); }
        bool IsMainDeclared() { return m_MainDeclared; }
        bool IsMainCurrentScope();

        std::pair<uint32_t, MainNode*> FindMain() const;

        bool FindVariable(const std::string& name, Type& type) const;
        InputPortNode* FindInputPort(const std::string& name) const;
        InputPortNode* FindInputPortByPortName(const std::string& portName) const;
        OutputPortNode* FindOutputPort(const std::string& name) const;
        OutputPortNode* FindOutputPortByPortName(const std::string& portName) const;
        InputNode* FindInput(const std::string& name) const;
        OutputNode* FindOutput(const std::string& name) const;
        bool RecordVariableForScope(const std::string& name, const Type& type, std::string& outError);
        bool AddUniform(const std::string& name, const Type& type, std::string& outError);

        const std::map<std::string, InputNode*>& GetInputs() const { return m_Inputs; }
        const std::map<std::string, OutputNode*>& GetOutputs() const { return m_Outputs; }
        const std::map<std::string, InputPortNode*>& GetInputPorts() const { return m_InputPorts; }
        const std::map<std::string, OutputPortNode*>& GetOutputPorts() const { return m_OutputPorts; }
        const std::vector<ASTNode*>& GetNodes() const { return m_AstNodes; }
        void InsertNode(size_t at, ASTNode* node);
        std::vector<AstScope>& GetScopeStack() { return m_ScopeStack; }
        std::map<std::string, Type>& GetGlobalVariables() { return m_GlobalVariables; }
        const std::map<std::string, Type>& GetUniformVariables() const { return m_Uniforms; }
        bool HasUniform(const std::string& name, Type type);

        bool m_MainDeclared = false;
        std::map<std::string, InputPortNode*> m_InputPorts;
        std::map<std::string, OutputPortNode*> m_OutputPorts;
        std::map<std::string, InputNode*> m_Inputs;
        std::map<std::string, OutputNode*> m_Outputs;
        std::vector<ASTNode*> m_AstNodes;
        std::vector<AstScope> m_ScopeStack;
        std::map<std::string, Type> m_Uniforms;
        std::map<std::string, Type> m_GlobalVariables;
    };
}
