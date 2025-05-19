#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"

namespace se::render::metal
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources(const render::VertexBuffer& vb) override;
        void DestroyPlatformResources() override;

    private:
        void ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;

        RenderPipelineStatePtr m_RenderPipelineState = nullptr;
        MTLDepthStencilStatePtr m_DepthStencilState = nullptr;
    };
}
#endif
