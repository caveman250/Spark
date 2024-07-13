#pragma once

namespace se::render
{
    class VertexBuffer;
}

namespace se::memory
{
    class Arena;
}

namespace se::asset::shader::ast
{
    class ASTNode;
    class Shader;
}

namespace se::asset::shader::compiler
{
    class ShaderCombiner
    {
    public:
        explicit ShaderCombiner(const render::VertexBuffer& vb);
        ast::Shader Combine(const ast::Shader& left, const ast::Shader& right, memory::Arena& tempStore);
        void ResolveCombinedShaderPorts(ast::Shader& shader, memory::Arena& arena);

    private:
        void ForEachChild(ast::ASTNode* node, std::function<void(ast::ASTNode* node)> func);

        uint8_t GetInputLoc(const std::string& inputName);
        uint8_t GetOutputLoc(const std::string& outputName);
        const render::VertexBuffer& m_VertexBuffer;
        std::vector<std::string> m_ConsumedOutputs = {};
    };
}
