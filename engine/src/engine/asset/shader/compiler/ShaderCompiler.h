#pragma once
#include "spark.h"
#include "engine/asset/shader/ShaderSettings.h"
#include "engine/asset/shader/ast/ConstantNode.h"
#include "engine/asset/shader/ast/VariableReferenceNode.h"

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
    class Shader;
}
namespace se::asset::shader
{
    class ShaderCompiler
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
    };

    template<typename T>
    void ShaderCompiler::ReplaceSettingReferenceWithConstant(Shader& shader, ast::ASTNode* node, const std::string& settingName, T constantValue)
    {
        static auto referenceType = reflect::TypeResolver<ast::VariableReferenceNode>::get();
        if (node->GetReflectType() == referenceType)
        {
            const auto referenceNode = static_cast<ast::VariableReferenceNode*>(node);
            if (strcmp(referenceNode->GetName().c_str(), settingName.data()) == 0)
            {
                const auto parent = node->m_Parent;
                auto it = std::ranges::find_if(parent->m_Children, [node](const auto& child){ return child.get() == node; });
                std::shared_ptr<ast::ASTNode> newVal = std::make_shared<ast::ConstantNode<T>>(constantValue);
                std::replace(parent->m_Children.begin(), parent->m_Children.end(), *it, newVal);
                return;
            }
        }

        node->ForEachChild([&shader, settingName, constantValue](ast::ASTNode* child)
        {
            ReplaceSettingReferenceWithConstant<T>(shader, child, settingName, constantValue);
        });
    }
}
