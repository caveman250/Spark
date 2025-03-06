#include "Material.h"

#include "Renderer.h"

namespace se::render
{
    void Material::Bind(const VertexBuffer& vb)
    {
        auto renderer = Renderer::Get<Renderer>();
        auto rs = renderer->GetCachedRenderState();
        if (rs.depthComp != m_RenderState.depthComp ||
            rs.stencilFunc != m_RenderState.stencilFunc ||
            rs.stencilReadMask != m_RenderState.stencilReadMask ||
            rs.stencilWriteMask != m_RenderState.stencilWriteMask)
        {
            ApplyDepthStencil(m_RenderState.depthComp, m_RenderState.stencilFunc, m_RenderState.stencilWriteMask, m_RenderState.stencilReadMask);
        }

        if (rs.srcBlend != m_RenderState.srcBlend ||
            rs.dstBlend != m_RenderState.dstBlend)
        {
            ApplyBlendMode(m_RenderState.srcBlend, m_RenderState.dstBlend);
        }

        renderer->SetLastRenderState(m_RenderState);

        if (m_RenderState.lit)
        {
            const auto& lightSetup = renderer->GetLightSetup();
            if (m_CachedLightSetup != lightSetup || !m_PlatformResourcesCreated)
            {
                m_ShaderSettings.SetSetting("numLights", static_cast<int>(lightSetup.pointLights.size()));
                DestroyPlatformResources();
                m_CachedLightSetup = lightSetup;
            }
        }

        if (!m_PlatformResourcesCreated)
        {
            CreatePlatformResources(vb);
            m_PlatformResourcesCreated = true;
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


