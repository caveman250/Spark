#include "ShaderGenerator.h"

#include <optional>

#include "ast/ASTNode.h"
#include "ast/InputAttributeNode.h"
#include "ast/OutputNode.h"
#include "ast/ShaderStage.h"
#include "engine/logging/Log.h"
#include "engine/memory/Arena.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"

std::optional<std::string> se::shader::ShaderGenerator::CompileShader(const std::string &filePath)
{
    logging::Log::Info("Compiling shader: %s", filePath.c_str());
    parser::Lexer lexer(filePath);
    parser::Parser parser(lexer);
    auto result = parser.Parse();
    if (std::holds_alternative<parser::ParseError>(result))
    {
        auto parseError = std::get<parser::ParseError>(result);
        logging::Log::Error("Shader Compile Error - %s - line:%u pos:%u: %s", "shader.vert", parseError.line,
                                parseError.pos, parseError.error.c_str());
        return std::nullopt;
    }
    logging::Log::Info("Sucess");
    return AstToGlsl(std::get<ast::ShaderStage>(result));
}

std::string se::shader::ShaderGenerator::AstToGlsl(const ast::ShaderStage &ast)
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

    for (const auto *node: ast.GetNodes())
    {
        node->ToGlsl(shader);
    }

    return shader.c_str();
}
