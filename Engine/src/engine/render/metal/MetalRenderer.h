#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/Renderer.h"

namespace se::render::metal
{
    class MetalRenderer : public Renderer
    {
    public:
        MetalRenderer();
        void Init() override;
        void Render(IWindow* window) override;

    private:
        void ApplyDepthCompare(DepthCompare::Type comp) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;
        void ApplyStencil(StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;
    };
}
#endif
