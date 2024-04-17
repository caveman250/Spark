#pragma once
#include "engine/shader/ast/ASTNode.h"

namespace se::shader::parser
{
    class PortInfo
    {
    public:
        ast::ASTNode* ResolvePort(const std::string& portName);
        void RecordPortWrite(const std::string& portName, ast::ASTNode* writtenTo);
        std::string GetLastPortVariableName(const std::string& portName);
    private:
        std::map<std::string, std::vector<ast::ASTNode*>> m_PortWriteStack;
    };
}
