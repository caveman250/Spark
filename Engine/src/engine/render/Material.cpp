#include "Material.h"

#include "Renderer.h"

namespace se::render
{
    void Material::Bind(const VertexBuffer& vb)
    {
        auto renderer = Renderer::Get();
        renderer->ApplyRenderState(m_RenderState);
        if (m_RenderState.lit)
        {
            const auto& lightSetup = renderer->GetLightSetup();
            if (m_CachedLightSetup != lightSetup || !m_PlatformResourcesCreated)
            {
                m_ShaderSettings.SetSetting("numLights", lightSetup.pointLights.size());
                DestroyPlatformResources();
                m_CachedLightSetup = lightSetup;
            }
        }

        if (!m_PlatformResourcesCreated)
        {
            CreatePlatformResources(vb);
            m_PlatformResourcesCreated = true;
        }

        if (m_UniformStorage.IsStale())
        {
            m_UniformStorage.Apply(this);
        }

        if (m_RenderState.lit)
        {
            const auto& lightSetup = Renderer::Get()->GetLightSetup();
            // TODO improve shader parser so i can just pass an array of structs
            std::vector<math::Vec3> pos;
            pos.resize(lightSetup.pointLights.size());
            std::transform(lightSetup.pointLights.begin(), lightSetup.pointLights.end(), pos.begin(), [](const PointLight& light){ return light.pos; });

            std::vector<math::Vec3> color;
            color.resize(lightSetup.pointLights.size());
            std::transform(lightSetup.pointLights.begin(), lightSetup.pointLights.end(), color.begin(), [](const PointLight& light){ return light.color; });

            SetUniform("lightPos", asset::shader::ast::AstType::Vec3, static_cast<int>(pos.size()), &pos[0]);
            SetUniform("lightColor", asset::shader::ast::AstType::Vec3, static_cast<int>(color.size()), &color[0]);
        }
    }

    void Material::CreatePlatformResources(const VertexBuffer&)
    {
        m_PlatformResourcesCreated = true;
    }

    void Material::DestroyPlatformResources()
    {
        m_PlatformResourcesCreated = false;
    }

    void Material::SetRenderState(const RenderState& state)
    {
        m_RenderState = state;
    }

    ShaderSettings& Material::GetShaderSettings()
    {
        return m_ShaderSettings;
    }

    Material:: Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : m_VertShaders(vertShaders)
        , m_FragShaders(fragShaders)
    {
    }
}


