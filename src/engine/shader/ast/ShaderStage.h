#pragma once
#include "spark.h"
#include "Types.h"

namespace se::shader::ast
{
    class OutputNode;
}

namespace se::shader::ast
{

}

namespace se::shader::ast
{
    class InputAttributeNode;
    class ASTNode;

    class AstScope
    {
    public:
        AstScope(ASTNode* node) : m_Node(node) {}
        ASTNode* m_Node;

        std::map<std::string, Type> m_Variables;
    };

    class ShaderStage
    {
    public:
        void AddInput(InputAttributeNode* node);
        void AddOutput(OutputNode* node);
        void AddNode(ASTNode* node);
        void PushScope(ASTNode* node);
        void PopScope();
        uint8_t ScopeDepth() { return m_ScopeStack.size(); }
        bool IsMainDeclared() { return m_MainDeclared; }
        bool IsMainCurrentScope();

        bool FindVariable(const std::string& name, Type& type) const;
        InputAttributeNode* FindInputAttribute(const std::string& name) const;
        OutputNode* FindOutput(const std::string& name) const;

        const std::map<std::string, InputAttributeNode*>& GetInputAttributes() const { return m_InputAttributes; }
        const std::map<std::string, OutputNode*>& GetOutputs() const { return m_Outputs; }
        const std::vector<ASTNode*>& GetNodes() const { return m_AstNodes; }
    private:
        bool m_MainDeclared = false;
        std::map<std::string, InputAttributeNode*> m_InputAttributes;
        std::map<std::string, OutputNode*> m_Outputs;
        std::vector<ASTNode*> m_AstNodes;
        std::vector<AstScope> m_ScopeStack;
    };
}
