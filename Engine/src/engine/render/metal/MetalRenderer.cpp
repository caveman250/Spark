#include "MetalRenderer.h"
#if METAL_RENDERER

#include "engine/render/RenderCommand.h"
#include "platform/IWindow.h"

namespace se::render::metal
{
    MetalRenderer::MetalRenderer()
    {

    }

    void MetalRenderer::Init()
    {
    }

    void MetalRenderer::Render(IWindow* window)
    {
        Renderer::Render(window);
        // TODO
    }

    void MetalRenderer::ApplyDepthCompare(DepthCompare::Type comp)
    {
        // TODO
    }

    void MetalRenderer::ApplyBlendMode(BlendMode::Type src, BlendMode::Type dst)
    {
        // TODO
    }

    void MetalRenderer::ApplyStencil(StencilFunc::Type func, uint32_t writeMask, uint32_t readMask)
    {
        // TODO
    }
}
#endif