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
        struct ShaderCompileContext;
        class ASTNode;
    }
}

namespace se::asset::shader::compiler
{
    class ShaderCombiner
    {
    public:
        explicit ShaderCombiner(const render::VertexBuffer& vb);
        static Shader Combine(const Shader& left, const Shader& right);
        void ResolveCombinedShaderPorts(Shader& shader, const ast::ShaderCompileContext& context) const;

    private:
        static void ForEachChild(const std::shared_ptr<ast::ASTNode>& node,
                                 const std::function<void(const std::shared_ptr<ast::ASTNode>& node)>& func);

        uint8_t GetInputLoc(const std::string& inputName) const;
        const render::VertexBuffer& m_VertexBuffer;
        std::vector<std::string> m_ConsumedOutputs = {};
    };
}
