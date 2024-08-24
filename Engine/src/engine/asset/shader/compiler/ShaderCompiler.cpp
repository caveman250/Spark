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
#include "engine/asset/shader/ShaderSettings.h"

namespace se::asset::shader
{
    std::optional<Shader> ShaderCompiler::CompileShader(const std::string& filePath)
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
        return std::get<Shader>(result);
    }

    std::optional<std::string> ShaderCompiler::GeneratePlatformShader(
        const std::vector<std::shared_ptr<Shader>> &shaderAssets, const ShaderSettings &settings,
        const render::VertexBuffer &vb,
        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> &textureResources)
    {
        std::optional<Shader> newShader = std::nullopt;
        std::vector<Shader> additionalStages;
        for (auto& shader : shaderAssets)
        {
            if (!newShader.has_value())
            {
                newShader = Shader(*shader.get());
            }
            else
            {
                additionalStages.emplace_back(Shader(*shader.get()));
            }
        }

        SPARK_ASSERT(newShader.has_value());

        auto combiner = compiler::ShaderCombiner(vb);
        for (auto additionalStage : additionalStages)
        {
            newShader = combiner.Combine(newShader.value(), additionalStage);
        }

        combiner.ResolveCombinedShaderPorts(newShader.value());

        if (!ResolveSettings(newShader.value(), settings))
        {
            return std::nullopt;
        }

        for (const auto& [uniformName, uniform] : newShader.value().GetUniformVariables())
        {
            if (uniform.type == ast::AstType::Sampler2D)
            {
                textureResources.push_back(std::make_pair(uniformName, nullptr));
            }
        }

        return AstToGlsl(newShader.value());
    }

    bool ShaderCompiler::ResolveSettings(Shader& shader, const ShaderSettings& settings)
    {
        for (const auto& [name, type] : shader.GetSettingVariables())
        {
            if (!SPARK_VERIFY(settings.HasDefinition(name)))
            {
                return false;
            }

            auto replacementText = settings.GetSettingText(name);
            std::map<std::string, std::string> renameMap = { { name, replacementText } };
            for (const auto& node : shader.GetNodes())
            {
                node->ApplyNameRemapping(renameMap);
            }

            for (auto& [uniformName, var] : shader.m_Uniforms)
            {
                if (var.arraySizeVariable == name)
                {
                    var.arraySizeConstant = std::stoi(replacementText);
                    var.arraySizeVariable = {};
                }
            }

            for (auto& [uniformName, var] : shader.m_GlobalVariables)
            {
                if (var.arraySizeVariable == name)
                {
                    var.arraySizeConstant = std::stoi(replacementText);
                    var.arraySizeVariable = {};
                }
            }

            for (auto& [inputName, input] : shader.m_Inputs)
            {
                if (input->GetVar().arraySizeVariable == name)
                {
                    input->SetVar(ast::Variable(input->GetVar().type, std::stoi(replacementText)));
                }
            }

            for (auto& [outputName, output] : shader.m_Outputs)
            {
                if (output->GetVar().arraySizeVariable == name)
                {
                    output->SetVar(ast::Variable(output->GetVar().type, std::stoi(replacementText)));
                }
            }
        }

        return true;
    }

    std::string ShaderCompiler::AstToGlsl(Shader &ast)
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

        for (const auto& [name, var] : ast.GetUniformVariables())
        {
            std::string arrayText = "";
            if (!var.arraySizeVariable.empty())
            {
                arrayText = std::format("[{}]", var.arraySizeVariable);
            }
            else if (var.arraySizeConstant != 0)
            {
                arrayText = std::format("[{}]", var.arraySizeConstant);
            }

            auto uniformText = std::format("uniform {0} {1}{2};\n", ast::TypeUtil::GetTypeGlsl(var.type), name, arrayText);
            shader.append(uniformText);
        }

        for (const auto& node: ast.GetNodes())
        {
            node->ToGlsl(shader);
        }

        return shader.c_str();
    }
}
