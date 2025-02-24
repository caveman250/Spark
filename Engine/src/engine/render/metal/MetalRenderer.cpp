#include "MetalRenderer.h"


#include <platform/mac/Window.h>

#include "Material.h"
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

        auto macWindow = static_cast<mac::Window*>(window);
        auto view = macWindow->GetView();
        m_CurrentRenderPassDescriptor = view->currentRenderPassDescriptor();
        m_CurrentCommandEncoder = pCmd->renderCommandEncoder(m_CurrentRenderPassDescriptor);
        
        m_CurrentCommandEncoder->setCullMode(MTL::CullModeBack);
        m_CurrentCommandEncoder->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

        Renderer::Render(window);

        m_CurrentCommandEncoder->endEncoding();
        m_CurrentCommandEncoder = nullptr;
        pCmd->presentDrawable(view->currentDrawable());
        pCmd->commit();

        pPool->release();

        m_CachedRenderState = {};
    }

    MTL::CompareFunction MetalRenderer::DepthCompareToMtl(DepthCompare::Type depthCompare)
    {
        switch (depthCompare)
        {
            case DepthCompare::Less:
                return MTL::CompareFunctionLess;
            case DepthCompare::LessEqual:
                return MTL::CompareFunctionLessEqual;
            case DepthCompare::Equal:
                return MTL::CompareFunctionEqual;
            case DepthCompare::Greater:
                return MTL::CompareFunctionGreater;
            case DepthCompare::GreaterEqual:
                return MTL::CompareFunctionGreaterEqual;
            case DepthCompare::None:
                return MTL::CompareFunctionAlways;
            default:
                SPARK_ASSERT(false, "MetalRenderer::DepthCompareToMtl - Unhandled DepthCompare");
                return MTL::CompareFunctionAlways;
        }
    }

    MTL::BlendFactor MetalRenderer::BlendModeToMtl(BlendMode::Type blendMode)
    {
        switch (blendMode) {
            case BlendMode::Zero:
                return MTL::BlendFactorZero;
            case BlendMode::One:
                return MTL::BlendFactorOne;
            case BlendMode::SrcColor:
                return MTL::BlendFactorSourceColor;
            case BlendMode::OneMinusSrcColor:
                return MTL::BlendFactorOneMinusSourceColor;
            case BlendMode::DstColor:
                return MTL::BlendFactorDestinationColor;
            case BlendMode::OneMinusDstColor:
                return MTL::BlendFactorOneMinusDestinationColor;
            case BlendMode::SrcAlpha:
                return MTL::BlendFactorSourceAlpha;
            case BlendMode::OneMinusSrcAlpha:
                return MTL::BlendFactorOneMinusSourceAlpha;
            case BlendMode::DstAlpha:
                return MTL::BlendFactorDestinationAlpha;
            case BlendMode::OneMinusDstAlpha:
                return MTL::BlendFactorOneMinusDestinationAlpha;
            case BlendMode::None:
                return MTL::BlendFactorZero;
            default:
                SPARK_ASSERT(false, "MetalRenderer::BlendModeToMtl - Unhandled BlendMode");
                return MTL::BlendFactorZero;
        }
    }
}
#endif
