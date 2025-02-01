#include "MetalRenderer.h"


#include <platform/mac/Window.h>
#if METAL_RENDERER

#include "engine/render/RenderCommand.h"
#include "platform/IWindow.h"

namespace MTL
{
    class RenderCommandEncoder;
    class RenderPassDescriptor;
}

namespace se::render::metal
{
    MetalRenderer::MetalRenderer()
    {

    }

    MetalRenderer::~MetalRenderer()
    {
        m_Device->release();
        m_CommandQueue->release();
    }

    void MetalRenderer::Init()
    {
        m_Device = MTL::CreateSystemDefaultDevice();
        m_CommandQueue = m_Device->newCommandQueue();
    }

    void MetalRenderer::Render(IWindow* window)
    {
        NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
        MTL::CommandBuffer* pCmd = m_CommandQueue->commandBuffer();

        Renderer::Render(window);

        auto macWindow = static_cast<mac::Window*>(window);
        auto view = macWindow->GetView();
        MTL::RenderPassDescriptor* pRpd = view->currentRenderPassDescriptor();
        MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);
        pEnc->endEncoding();
        pCmd->presentDrawable(view->currentDrawable());
        pCmd->commit();

        pPool->release();
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