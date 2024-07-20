#include "ShaderCompiler.h"

#include <optional>

#include "engine/asset/shader/ast/ASTNode.h"
#include "engine/asset/shader/ast/InputAttributeNode.h"
#include "engine/asset/shader/ast/OutputNode.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "ShaderCombiner.h"
#include "engine/memory/Arena.h"
#include "Lexer.h"
#include "Parser.h"

std::optional<std::string> se::asset::shader::ShaderCompiler::CompileShader(const std::vector<std::string>& filePaths, const render::VertexBuffer& vb)
{
    std::optional<ast::Shader> firstStage = std::nullopt;
    std::vector<ast::Shader> additionalStages;
    for (auto& path : filePaths)
    {
        debug::Log::Info("Compiling shader: {0}", path.c_str());
        compiler::Lexer lexer(path);
        compiler::Parser parser(lexer);
        auto result = parser.Parse();
        if (std::holds_alternative<compiler::ParseError>(result))
        {
            auto parseError = std::get<compiler::ParseError>(result);
            debug::Log::Error("Shader Compile Error - {0} - line:{1} pos:{2}: {3}", path, parseError.line,
                                    parseError.pos, parseError.error);
            return std::nullopt;
        }
        debug::Log::Info("Sucess");

        if (!firstStage.has_value())
        {
            firstStage = std::get<ast::Shader>(result);
        }
        else
        {
            additionalStages.push_back(std::get<ast::Shader>(result));
        }
    }

    auto combiner = compiler::ShaderCombiner(vb);
    for (auto& additionalStage : additionalStages)
    {
        firstStage = combiner.Combine(firstStage.value(), additionalStage);
    }

    combiner.ResolveCombinedShaderPorts(firstStage.value());

    return AstToGlsl(firstStage.value());
}

std::string se::asset::shader::ShaderCompiler::AstToGlsl(ast::Shader &ast)
{
#if 0
    for (const auto *node: ast.GetNodes())
    {
        node->DebugPrint(0);
    }
#endif

    memory::Arena arena;
    memory::ArenaAllocator<char> alloc(arena);
    string::ArenaString shader(alloc);

    shader.append("#version 330 core\n"); //TODO this is old right?


    for (const auto &[name, node]: ast.GetInputs())
    {
        node->ToGlsl(shader);
    }

    for (const auto &[name, node]: ast.GetOutputs())
    {
        node->ToGlsl(shader);
    }

    for (const auto& [name, type] : ast.GetUniformVariables())
    {
        shader.append(std::format("uniform {0} {1};\n", ast::TypeUtil::GetTypeGlsl(type), name));
    }

    for (const auto& node: ast.GetNodes())
    {
        node->ToGlsl(shader);
    }

    return shader.c_str();
}
