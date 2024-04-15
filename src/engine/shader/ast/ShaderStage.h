#pragma once
#include "spark.h"

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
        // todo variable declarations
    };

    class ShaderStage
    {
    public:
        void AddInput(InputAttributeNode* node);
        void AddOutput(OutputNode* node);
        void AddNode(ASTNode* node);
        void PushScope(ASTNode* node);
        void PopScope();
        const std::map<std::string, InputAttributeNode*>& GetInputAttributes() const { return m_InputAttributes; }
        const std::map<std::string, OutputNode*>& GetOutputs() const { return m_Outputs; }
        const std::vector<ASTNode*>& GetNodes() const { return m_AstNodes; }
        ASTNode* FindInput(const std::string& name) const;
        ASTNode* FindOutput(const std::string& name) const;
    private:
        std::map<std::string, InputAttributeNode*> m_InputAttributes;
        std::map<std::string, OutputNode*> m_Outputs;
        std::vector<ASTNode*> m_AstNodes;
        std::vector<AstScope> m_CurrentScope;
    };
}
