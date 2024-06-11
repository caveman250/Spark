#pragma once

namespace se::memory
{
    class Arena;
}

namespace se::shader::ast
{
    class ASTNode;
    class ShaderStage;
}

namespace se::shader::compiler
{
    class ShaderStageCombiner
    {
    public:
        ast::ShaderStage Combine(const ast::ShaderStage& left, const ast::ShaderStage& right, memory::Arena& tempStore);
        void ResolveCombinedShaderPorts(ast::ShaderStage& shader, memory::Arena& arena);
        void ForEachChild(ast::ASTNode* node, std::function<void(ast::ASTNode* node)> func);

    private:
        uint8_t GetInputLoc(const std::string& inputName);
        uint8_t GetOutputLoc(const std::string& outputName);
        std::vector<std::string> m_ConsumedInputs;
        std::vector<std::string> m_ConsumedOutputs;
    };
}
