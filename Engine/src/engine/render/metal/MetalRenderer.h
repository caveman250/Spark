#pragma once
#include "MTL_fwd.h"
#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/Renderer.h"

namespace se::render::metal
{
    class MetalRenderer : public Renderer
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;
        RenderAPI::Type GetRenderAPIType() const override { return RenderAPI::Metal; }
        void Init() override;
        void Render(IWindow* window) override;

        MTL::Device* GetDevice() const { return m_Device; }
        MTL::RenderCommandEncoder* GetCurrentCommandEncoder() const { return m_CurrentCommandEncoder; }


    private:
        void ApplyDepthCompare(DepthCompare::Type comp) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;
        void ApplyStencil(StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;

        MTL::Device* m_Device = nullptr;
        MTL::CommandQueue* m_CommandQueue = nullptr;
        MTL::RenderCommandEncoder* m_CurrentCommandEncoder = nullptr;
    };
}
#endif
