#include "ShaderGenerator.h"

#include <optional>

#include "ast/ASTNode.h"
#include "ast/InputAttributeNode.h"
#include "ast/OutputNode.h"
#include "ast/ShaderStage.h"
#include "ast/TypeUtil.h"
#include "compiler/ShaderStageCombiner.h"
#include "engine/logging/Log.h"
#include "engine/memory/Arena.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"

std::optional<std::string> se::shader::ShaderGenerator::CompileShader(const std::vector<std::string>& filePaths)
{
    memory::Arena arena;
    std::optional<ast::ShaderStage> firstStage = std::nullopt;
    std::vector<ast::ShaderStage> additionalStages;
    for (auto& path : filePaths)
    {
        logging::Log::Info("Compiling shader: {0}", path.c_str());
        parser::Lexer lexer(path);
        parser::Parser parser(lexer, &arena);
        auto result = parser.Parse();
        if (std::holds_alternative<parser::ParseError>(result))
        {
            auto parseError = std::get<parser::ParseError>(result);
            logging::Log::Error("Shader Compile Error - {0} - line:{1} pos:{2}: {3}", path, parseError.line,
                                    parseError.pos, parseError.error);
            return std::nullopt;
        }
        logging::Log::Info("Sucess");

        if (!firstStage.has_value())
        {
            firstStage = std::get<ast::ShaderStage>(result);
        }
        else
        {
            additionalStages.push_back(std::get<ast::ShaderStage>(result));
        }
    }

    for (auto& additionalStage : additionalStages)
    {
        firstStage = compiler::ShaderStageCombiner::Combine(firstStage.value(), additionalStage, arena);
    }

    compiler::ShaderStageCombiner::ResolveCombinedShaderPorts(firstStage.value(), arena);

    return AstToGlsl(firstStage.value());
}

std::string se::shader::ShaderGenerator::AstToGlsl(ast::ShaderStage &ast)
{
    memory::Arena arena;
    memory::ArenaAllocator<char> alloc(arena);
    string::ArenaString shader(alloc);

    shader.append("#version 330 core\n"); //TODO this is old right?

    for (const auto &[name, node]: ast.GetInputAttributes())
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

    for (const auto *node: ast.GetNodes())
    {
        node->ToGlsl(shader);
    }

    return shader.c_str();
}
