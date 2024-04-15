#pragma once

namespace se::shader
{
    namespace ast
    {
        class ShaderStage;
    }

    class ShaderGenerator
    {
    public:
        static std::string AstToGlsl(const ast::ShaderStage& ast);
    };
}
