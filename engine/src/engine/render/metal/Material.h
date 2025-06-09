#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/Material.h"

namespace se::render::metal
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind(const VertexBuffer&) override;
        void CreatePlatformResources(const VertexBuffer& vb) override;
        void DestroyPlatformResources() override;

    private:
        void ApplyDepthStencil(DepthCompare comp, StencilFunc src, uint32_t writeMask, uint32_t readMask) override;
        void ApplyBlendMode(BlendMode src, BlendMode dest) override;

        RenderPipelineStatePtr m_RenderPipelineState = nullptr;
        MTLDepthStencilStatePtr m_DepthStencilState = nullptr;
    };
}
#endif
