#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/Material.h"

namespace se::render::metal
{
    class MaterialPlatformResources : public render::MaterialPlatformResources
    {
    public:
        void Bind() override;
        void CreatePlatformResources(const std::string& vert,
            const std::string& frag,
            const RenderState& rs) override;
        void DestroyPlatformResources() override;

    private:
        void ApplyDepthStencil(const RenderState& rs) override;
        void ApplyBlendMode(BlendMode src, BlendMode dest) override;

        RenderPipelineStatePtr m_RenderPipelineState = nullptr;
        MTLDepthStencilStatePtr m_DepthStencilState = nullptr;
    };
}
#endif
