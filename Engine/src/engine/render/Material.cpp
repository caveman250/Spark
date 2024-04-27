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

    Material:: Material(const std::vector<std::string>& vertPaths, const std::vector<std::string>& fragPaths)
        : m_VertShaderPaths(vertPaths)
        , m_FragShaderPaths(fragPaths)
    {
    }
}


