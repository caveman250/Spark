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

    Material:: Material(const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& vertShaders,
                         const std::vector<std::shared_ptr<asset::shader::ast::Shader>>& fragShaders)
        : m_VertShaders(vertShaders)
        , m_FragShaders(fragShaders)
    {
    }
}


