#include "MaterialEditor.h"

#include "engine/render/Material.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(render::Material, MaterialEditor, Material);

    void MaterialEditor::Update()
    {
        ClassEditor::Update();

        auto* material = static_cast<render::Material*>(m_Value);
        ShaderSettings& settings = material->GetShaderSettings();
        const auto& vertShaders = material->GetVertShaders();
        int numValidShaders = std::accumulate(vertShaders.begin(), vertShaders.end(), int{}, [](int val, const asset::AssetReference<asset::Shader>& asset)
        {
            int rhs = asset.IsSet() ? 1 : 0;
            return val + rhs;
        });
        if (m_CachedVertexShaderCount != numValidShaders)
        {
            for (const auto& shader : vertShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& setting : shaderAsset->GetSettingVariables())
                    {
                        if (!setting.second.internal && !settings.HasDefinition(setting.first))
                        {
                            settings.SetSettingDefault(setting.first, setting.second.var.type);
                        }
                    }
                }
            }
            m_CachedVertexShaderCount = numValidShaders;
        }

        const auto& fragShaders = material->GetFragShaders();
        numValidShaders = std::accumulate(fragShaders.begin(), fragShaders.end(), int{}, [](int val, const asset::AssetReference<asset::Shader>& asset)
        {
            int rhs = asset.IsSet() ? 1 : 0;
            return val + rhs;
        });
        if (m_CachedFragmentShaderCount != numValidShaders)
        {
            for (const auto& shader : fragShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& setting : shaderAsset->GetSettingVariables())
                    {
                        if (!setting.second.internal && !settings.HasDefinition(setting.first))
                        {
                            settings.SetSettingDefault(setting.first, setting.second.var.type);
                        }
                    }
                }
            }
            m_CachedFragmentShaderCount = numValidShaders;
        }

        render::UniformStorage& uniforms = material->GetUniformDefaults();
        int numUniforms = 0;
        for (const auto& shader : vertShaders)
        {
            if (shader.IsSet())
            {
                numUniforms += static_cast<int>(shader.GetAsset()->GetUniformVariables().size());
            }
        }
        for (const auto& shader : fragShaders)
        {
            if (shader.IsSet())
            {
                numUniforms += static_cast<int>(shader.GetAsset()->GetUniformVariables().size());
            }
        }

        if (m_CachedUniformsCount != numUniforms)
        {
            for (const auto& shader : vertShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& uniform : shaderAsset->GetUniformVariables())
                    {
                        if (!uniform.second.internal && !uniforms.HasValue(uniform.first))
                        {
                            uniforms.SetValueDefault(uniform.first, uniform.second.var.type);
                        }
                    }
                }
            }
            for (const auto& shader : fragShaders)
            {
                if (shader.IsSet())
                {
                    const auto& shaderAsset = shader.GetAsset();
                    for (const auto& uniform : shaderAsset->GetUniformVariables())
                    {
                        if (!uniform.second.internal && !uniforms.HasValue(uniform.first))
                        {
                            uniforms.SetValueDefault(uniform.first, uniform.second.var.type);
                        }
                    }
                }
            }
            m_CachedUniformsCount = numUniforms;
        }
    }
}
