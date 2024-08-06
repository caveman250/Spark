#include "Material.h"

#include "Renderer.h"

namespace se::render
{
    void Material::Bind()
    {
        Renderer::Get()->ApplyRenderState(m_RenderState);
    }

    void Material::SetRenderState(const RenderState& state)
    {
        m_RenderState = state;
    }

    void Material::SetShaderSettings(const ShaderSettings& settings)
    {
        m_ShaderSettings = settings;
    }

    Material:: Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : m_VertShaders(vertShaders)
        , m_FragShaders(fragShaders)
    {
    }
}


