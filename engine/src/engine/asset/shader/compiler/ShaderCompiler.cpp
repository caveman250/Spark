#include "ShaderCompiler.h"

#include "Lexer.h"
#include "Parser.h"
#include "ShaderCombiner.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/ShaderSettings.h"
#include "engine/asset/shader/ast/ASTNode.h"
#include "engine/asset/shader/ast/OutputNode.h"
#include "engine/asset/shader/ast/ShaderCompileContext.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "engine/memory/Arena.h"
#include "engine/render/Renderer.h"

namespace se::asset::shader
{
    std::optional<Shader> ShaderCompiler::CompileShader(const std::string& filePath)
    {
        debug::Log::Info("Compiling shader: {0}", filePath.c_str());
        const compiler::Lexer lexer(filePath);
        compiler::Parser parser(lexer);
        auto result = parser.Parse();
        if (std::holds_alternative<compiler::ParseError>(result))
        {
            auto parseError = std::get<compiler::ParseError>(result);
            debug::Log::Error("Shader Compile Error - {0} - line:{1} pos:{2}: {3}", filePath, parseError.line,
                                    parseError.pos, parseError.error);
            return std::nullopt;
        }
        debug::Log::Info("Success");
        return std::get<Shader>(result);
    }

    std::vector<std::pair<std::string, ast::Variable>> ShaderCompiler::GatherUsedUniforms(const std::vector<std::shared_ptr<Shader>>& shaderAssets)
    {
        std::vector<std::pair<std::string, ast::Variable>> ret;

        for (const auto& shader : shaderAssets)
        {
            const auto& uniforms = shader->GetUniformVariables();
            for (const auto& uniform : uniforms)
            {
                ret.push_back(uniform);
            }
        }

        return ret;
    }

    std::optional<std::string> ShaderCompiler::GeneratePlatformShader(
        const std::vector<std::shared_ptr<Shader>> &shaderAssets, const ShaderSettings &settings,
        const render::VertexBuffer &vb, ast::ShaderCompileContext& context)
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

        context.currentShader = &newShader.value();
        combiner.ResolveCombinedShaderPorts(newShader.value(), context);

        if (!ResolveSettings(newShader.value(), settings))
        {
            return std::nullopt;
        }
        
        if (context.currentShader->GetType() == ShaderType::Vertex)
        {
            context.vertShader = std::make_shared<Shader>(*context.currentShader);
        }
        else if (context.currentShader->GetType() == ShaderType::Fragment)
        {
            context.fragShader = std::make_shared<Shader>(*context.currentShader);
        }

        const auto renderApiType = render::Renderer::Get<render::Renderer>()->GetRenderAPIType();
        if (renderApiType == render::RenderAPI::OpenGL)
        {
            return AstToGlsl(newShader.value(), context);
        }
        else if (renderApiType == render::RenderAPI::Metal)
        {
            return AstToMtl(newShader.value(), context);
        }
        else
        {
            SPARK_ASSERT(false, "ShaderCompiler::GeneratePlatformShader - Unhandled RenderAPI");
            return "";
        }
    }

    bool ShaderCompiler::ResolveSettings(Shader& shader, const ShaderSettings& settings)
    {
        for (const auto& name: shader.GetSettingVariables() | std::views::keys)
        {
            if (!SPARK_VERIFY(settings.HasDefinition(name)))
            {
                return false;
            }

            auto replacementText = settings.GetSettingText(name);
            for (const auto& node : shader.GetNodes())
            {
                auto value = settings.GetSettingValue(name);
                std::visit([&shader, node, name](auto&& arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>)
                        ReplaceSettingReferenceWithConstant(shader, node.get(), name, arg);
                    else if constexpr (std::is_same_v<T, float>)
                        ReplaceSettingReferenceWithConstant(shader, node.get(), name, arg);
                    else if constexpr (std::is_same_v<T, math::Vec2>)
                        ReplaceSettingReferenceWithConstant(shader, node.get(), name, arg);
                    else if constexpr (std::is_same_v<T, math::Vec3>)
                        ReplaceSettingReferenceWithConstant(shader, node.get(), name, arg);
                    else if constexpr (std::is_same_v<T, math::Vec4>)
                        ReplaceSettingReferenceWithConstant(shader, node.get(), name, arg);
                    else
                        SPARK_ASSERT(false, "Unhandled Shader Setting Value type!");
                }, value);
            }

            for (auto& var: shader.m_Uniforms | std::views::values)
            {
                if (var.arraySizeVariable == name)
                {
                    var.arraySizeConstant = std::stoi(replacementText);
                    var.arraySizeVariable = {};
                }
            }

            for (auto& var: shader.m_GlobalVariables | std::views::values)
            {
                if (var.arraySizeVariable == name)
                {
                    var.arraySizeConstant = std::stoi(replacementText);
                    var.arraySizeVariable = {};
                }
            }

            for (const auto& input: shader.m_Inputs | std::views::values)
            {
                if (input->GetVar().arraySizeVariable == name)
                {
                    input->SetVar(ast::Variable(input->GetVar().type, std::stoi(replacementText)));
                }
            }

            for (const auto& output: shader.m_Outputs | std::views::values)
            {
                if (output->GetVar().arraySizeVariable == name)
                {
                    output->SetVar(ast::Variable(output->GetVar().type, std::stoi(replacementText)));
                }
            }
        }

        return true;
    }

    std::string ShaderCompiler::AstToGlsl(const Shader &ast, ast::ShaderCompileContext& context)
    {
#if 0
        for (const auto *node: ast.GetNodes())
        {
            node->DebugPrint(0);
        }
#endif

        memory::Arena arena;
        const memory::ArenaAllocator<char> alloc(arena);
        string::ArenaString shader(alloc);

        shader.append("#version 410\n");


        for (const auto& node: ast.GetInputs() | std::views::values)
        {
            node->ToGlsl(context, shader);
        }

        for (const auto& node: ast.GetOutputs() | std::views::values)
        {
            node->ToGlsl(context, shader);
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

            auto uniformText = std::format("uniform {0} {1}{2};\n", ast::TypeUtil::TypeToGlsl(var.type), name, arrayText);
            shader.append(uniformText);
        }

        for (const auto& node: ast.GetNodes())
        {
            node->ToGlsl(context, shader);
        }

        return shader.c_str();
    }

    std::string ShaderCompiler::AstToMtl(const Shader& ast, ast::ShaderCompileContext& context)
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

        shader.append("#include <metal_stdlib>\nusing namespace metal;\n");

        shader.append(string::ArenaFormat("struct v2f\n{{\nfloat4 {0} [[position]];\n", alloc, context.vertexPositionOutputName));
        if (ast.GetType() == ShaderType::Vertex)
        {
            for (const auto& node: ast.GetOutputs() | std::views::values)
            {
                node->ToMtl(context, shader);
            }
        }
        else if (ast.GetType() == ShaderType::Fragment)
        {
            for (const auto& node: ast.GetInputs() | std::views::values)
            {
                node->ToMtl(context, shader);
            }
        }
        shader.append("};\n");

        if (ast.GetType() == ShaderType::Fragment)
        {
            shader.append("struct FragmentOutput\n{\n");
            int i = 0;
            for (const auto& [name, node] : ast.GetOutputs())
            {
                shader += string::ArenaFormat("{} {} [[color({})]];\n", alloc, ast::TypeUtil::TypeToMtl(node->GetVar().type), name, i++);
            }
            shader.append("};\n");
        }

        shader.append("struct UniformData\n{\n");

        for (const auto& [name, var] : ast.GetUniformVariables())
        {
            if (var.type == ast::AstType::Sampler2D)
            {
                // Metal defines these as fragment shader main args.
                continue;
            }
            std::string arrayText = "";
            if (!var.arraySizeVariable.empty())
            {
                arrayText = std::format("[{}]", var.arraySizeVariable);
            }
            else if (var.arraySizeConstant != 0)
            {
                arrayText = std::format("[{}]", var.arraySizeConstant);
            }

            auto uniformText = std::format("{0} {1}{2};\n", ast::TypeUtil::TypeToMtl(var.type), name, arrayText);
            shader.append(uniformText);
        }

        shader.append("};\n");

        for (const auto& node: ast.GetNodes())
        {
            node->ToMtl(context, shader);
        }

        return shader.c_str();
    }
}
