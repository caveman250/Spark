#pragma once

namespace se::render
{
    class VertexBuffer;
}

namespace se::memory
{
    class Arena;
}

namespace se::asset
{
    class Shader;

    namespace shader::ast
    {
        class ASTNode;
    }
}

namespace se::asset::shader::compiler
{
    class ShaderCombiner
    {
    public:
        explicit ShaderCombiner(const render::VertexBuffer& vb);
        Shader Combine(const Shader& left, const Shader& right);
        void ResolveCombinedShaderPorts(Shader& shader);

    private:
        void ForEachChild(const std::shared_ptr<ast::ASTNode>& node, std::function<void(const std::shared_ptr<ast::ASTNode>& node)> func);

        uint8_t GetInputLoc(const std::string& inputName);
        uint8_t GetOutputLoc(const std::string& outputName);
        const render::VertexBuffer& m_VertexBuffer;
        std::vector<std::string> m_ConsumedOutputs = {};
    };
}
