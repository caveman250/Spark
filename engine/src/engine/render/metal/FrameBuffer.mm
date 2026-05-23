#include "FrameBuffer.h"
#import "engine/asset/texture/Texture.h"
#import "engine/math/math.h"
#import "engine/render/Renderer.h"
#import "engine/render/metal/TextureResource.h"
#import "MetalRenderer.h"
#include "engine/render/FrameBuffer.h"
#import <easy/profiler.h>
#import <Metal/Metal.h>

namespace se::render
{
    std::shared_ptr<FrameBuffer> FrameBuffer::CreateFrameBuffer(const math::IntVec2& size)
    {
        return std::make_shared<metal::FrameBuffer>(size);
    }
}

namespace se::render::metal
{
    FrameBuffer::FrameBuffer(const math::IntVec2& size)
        : render::FrameBuffer(size)
    {
    }

    void FrameBuffer::PreRender()
    {
      	EASY_BLOCK("FrameBuffer::PreRender");
        auto renderer = Renderer::Get<MetalRenderer>();
        MTLRenderPassDescriptor* desc = [[MTLRenderPassDescriptor alloc] init];

        m_ColourAttachmentDescriptor = [[desc colorAttachments] objectAtIndexedSubscript:0];
        const auto& colorTextureResource = static_pointer_cast<TextureResource>(m_ColorTexture->GetPlatformResource());
        [m_ColourAttachmentDescriptor setTexture:colorTextureResource->GetMetalResource()];
        [m_ColourAttachmentDescriptor setClearColor:MTLClearColorMake(m_ClearColour.x, m_ClearColour.y, m_ClearColour.z, m_ClearColour.w)];
        [m_ColourAttachmentDescriptor setLoadAction:MTLLoadActionClear];

        MTLRenderPassDepthAttachmentDescriptor* depthAttachment = [desc depthAttachment];
        const auto& depthTextureResource = static_pointer_cast<TextureResource>(m_DepthTexture->GetPlatformResource());
        [depthAttachment setTexture:depthTextureResource->GetMetalResource()];

        auto commandQueue = renderer->GetCommandQueue();
        m_CommandBuffer = [commandQueue commandBuffer];
        m_RenderCommandEncoder = [m_CommandBuffer renderCommandEncoderWithDescriptor:desc];
        [m_RenderCommandEncoder setCullMode:MTLCullModeBack];
        [m_RenderCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    }

    void FrameBuffer::Bind()
    {
    }

    void FrameBuffer::UnBind()
    {
    }

    void FrameBuffer::Commit()
    {
        [m_RenderCommandEncoder endEncoding];
        [m_CommandBuffer commit];
    }

    void FrameBuffer::SetClearColour(const math::Vec4& colour)
    {
        // TODO currently doesnt apply until next frame.
        m_ClearColour = colour;
    }
}
