#include "Material.h"

#include "Renderer.h"
#include "engine/asset/shader/ast/NameGenerator.h"
#include "engine/asset/shader/ast/ShaderCompileContext.h"
#include "engine/asset/shader/compiler/ShaderCompiler.h"

namespace se::render
{
    void Material::Bind(const VertexBuffer& vb)
    {
        auto renderer = Renderer::Get<Renderer>();
        auto boundMat = renderer->GetBoundMaterial();
        if (boundMat == this)
        {
            return;
        }

        renderer->SetBoundMaterial(this);

        if (!m_PlatformResources)
        {
            m_PlatformResources = CreateMaterialPlatformResources();
        }

        auto rs = renderer->GetCachedRenderState();
        if (rs.depthComp != m_RenderState.depthComp ||
            rs.stencilFunc != m_RenderState.stencilFunc ||
            rs.stencilReadMask != m_RenderState.stencilReadMask ||
            rs.stencilWriteMask != m_RenderState.stencilWriteMask)
        {
            m_PlatformResources->ApplyDepthStencil(m_RenderState);
        }

        if (rs.srcBlend != m_RenderState.srcBlend ||
            rs.dstBlend != m_RenderState.dstBlend)
        {
            m_PlatformResources->ApplyBlendMode(m_RenderState.srcBlend, m_RenderState.dstBlend);
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

        m_PlatformResources->Bind();
    }

    void Material::CreatePlatformResources(const VertexBuffer& vb)
    {
        asset::shader::ast::ShaderCompileContext context = {
            nullptr, nullptr, nullptr, asset::shader::ast::NameGenerator::GetName(), {}, {}
        };

        auto vertShaders = LoadAssetList<asset::Shader>(m_VertShaders);
        auto fragShaders = LoadAssetList<asset::Shader>(m_FragShaders);

        auto fragUniforms = asset::shader::ShaderCompiler::GatherUsedUniforms(fragShaders);
        context.fragmentShaderUniforms = fragUniforms;
        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(
            vertShaders, m_ShaderSettings, vb, context);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(
            fragShaders, m_ShaderSettings, vb, context);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }

        m_VertUniforms = context.vertShader->GetUniformVariables();
        m_FragUniforms = context.fragShader->GetUniformVariables();

        SPARK_ASSERT(!m_VertUniforms.empty());
        m_PlatformResources->CreatePlatformResources(vert.value(), frag.value(), m_RenderState);
        m_PlatformResourcesCreated = true;
    }

    void Material::DestroyPlatformResources()
    {
        m_PlatformResources->DestroyPlatformResources();
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

    Material:: Material(const std::vector<asset::AssetReference<asset::Shader>>& vertShaders,
                        const std::vector<asset::AssetReference<asset::Shader>>& fragShaders)
        : m_VertShaders(vertShaders)
        , m_FragShaders(fragShaders)
    {
    }
}


