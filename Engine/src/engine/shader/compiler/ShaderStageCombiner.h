#pragma once

namespace se::render
{
    class VertexBuffer;
}

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
        explicit ShaderStageCombiner(const render::VertexBuffer& vb);
        ast::ShaderStage Combine(const ast::ShaderStage& left, const ast::ShaderStage& right, memory::Arena& tempStore);
        void ResolveCombinedShaderPorts(ast::ShaderStage& shader, memory::Arena& arena);
        void ForEachChild(ast::ASTNode* node, std::function<void(ast::ASTNode* node)> func);

    private:
        uint8_t GetInputLoc(const std::string& inputName);
        uint8_t GetOutputLoc(const std::string& outputName);
        const render::VertexBuffer& m_VertexBuffer;
        std::vector<std::string> m_ConsumedOutputs = {};
    };
}
