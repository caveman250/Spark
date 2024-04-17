#pragma once

namespace se::memory
{
    class Arena;
}

namespace se::shader::ast
{
    class ShaderStage;
}

namespace se::shader::compiler
{
    class ShaderStageCombiner
    {
    public:
        static ast::ShaderStage Combine(const ast::ShaderStage& left, const ast::ShaderStage& right, memory::Arena& tempStore);
        static void ResolveCombinedShaderPorts(ast::ShaderStage& shader, memory::Arena& arena);
    };
}
