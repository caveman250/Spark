module;
#include <algorithm>

#include "spark.h"
#include "engine/reflect/TypeResolver.h"

export module Spark.Asset.Shader:ShaderCompiler;
import :ASTNode;

namespace se
{
    class ShaderSettings;
}

namespace se::render
{
    class VertexBuffer;
}
namespace se::asset
{
    export class Shader;
}
namespace se::asset::shader
{
    namespace ast
    {
        template <typename T>
        class ConstantNode;
        struct ShaderCompileContext;
        struct Variable;

    }

    export class ShaderCompiler
    {
    public:
        static std::optional<Shader> CompileShader(const std::string& filePath);
        static std::vector<std::pair<std::string, ast::Variable>> GatherUsedUniforms(const std::vector<std::shared_ptr<Shader>>& shaderAssets);
        static std::optional<std::string> GeneratePlatformShader(const std::vector<std::shared_ptr<Shader>>& shaderAssets,
                                                                    const ShaderSettings& settings,
                                                                    const render::VertexBuffer& vb,
                                                                    ast::ShaderCompileContext& context);

        static bool ResolveSettings(Shader& shader, const ShaderSettings& settings);
        template <typename T>
        static void ReplaceSettingReferenceWithConstant(Shader& shader, ast::ASTNode* node, const std::string& settingName, T constantValue);
        static std::string AstToGlsl(const Shader& ast, ast::ShaderCompileContext& context);
        static std::string AstToMtl(const Shader& ast, ast::ShaderCompileContext& context);
        static std::optional<std::vector<std::shared_ptr<ast::ASTNode>>::iterator> FindVariable(Shader& shader, ast::ASTNode* node, const std::string& settingName, ast::ASTNode** parent);
    };

    template<typename T>
    void ShaderCompiler::ReplaceSettingReferenceWithConstant(Shader& shader, ast::ASTNode* node, const std::string& settingName, T constantValue)
    {
        ast::ASTNode* parent = nullptr;
        auto it = FindVariable(shader, node, settingName, &parent);
        if (it.has_value())
        {
            std::shared_ptr<ast::ASTNode> newVal = std::make_shared<ast::ConstantNode<T>>(constantValue);
            std::replace(parent->m_Children.begin(), parent->m_Children.end(), **it, newVal);
            return;
        }

        node->ForEachChild([&shader, settingName, constantValue](ast::ASTNode* child)
        {
            ReplaceSettingReferenceWithConstant<T>(shader, child, settingName, constantValue);
        });
    }
}
