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

namespace se::asset::shader
{
    std::optional<ast::Shader> ShaderCompiler::CompileShader(const std::string& filePath)
    {
        debug::Log::Info("Compiling shader: {0}", filePath.c_str());
        compiler::Lexer lexer(filePath);
        compiler::Parser parser(lexer);
        auto result = parser.Parse();
        if (std::holds_alternative<compiler::ParseError>(result))
        {
            auto parseError = std::get<compiler::ParseError>(result);
            debug::Log::Error("Shader Compile Error - {0} - line:{1} pos:{2}: {3}", filePath, parseError.line,
                                    parseError.pos, parseError.error);
            return std::nullopt;
        }
        debug::Log::Info("Sucess");
        return std::get<ast::Shader>(result);
    }

    std::optional<std::string> ShaderCompiler::GeneratePlatformShader(const std::vector<std::shared_ptr<ast::Shader>>& shaderAssets, const render::VertexBuffer& vb)
    {
        std::optional<ast::Shader> newShader = std::nullopt;
        std::vector<ast::Shader> additionalStages;
        for (auto& shader : shaderAssets)
        {
            if (!newShader.has_value())
            {
                newShader = ast::Shader(*shader.get());
            }
            else
            {
                additionalStages.emplace_back(ast::Shader(*shader.get()));
            }
        }

        SPARK_ASSERT(newShader.has_value());

        auto combiner = compiler::ShaderCombiner(vb);
        for (auto additionalStage : additionalStages)
        {
            newShader = combiner.Combine(newShader.value(), additionalStage);
        }

        combiner.ResolveCombinedShaderPorts(newShader.value());

        return AstToGlsl(newShader.value());
    }

    std::string ShaderCompiler::AstToGlsl(ast::Shader &ast)
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
}
