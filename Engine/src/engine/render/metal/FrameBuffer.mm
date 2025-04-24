#include "FrameBuffer.h"
#import "engine/asset/texture/Texture.h"
#import "engine/math/math.h"
#import "engine/render/Renderer.h"
#import "engine/render/metal/TextureResource.h"
#import "MetalRenderer.h"
#import <Metal/Metal.h>

namespace se::render
{
    std::shared_ptr<FrameBuffer> FrameBuffer::CreateFrameBuffer()
    {
        return std::make_shared<metal::FrameBuffer>();
    }
}

namespace se::render::metal
{
    FrameBuffer::FrameBuffer()
    {
        m_ColorTexture = asset::Texture::Create(1280, 720, asset::texture::Format::BGRA8, asset::texture::Usage::RenderTarget);
        m_DepthTexture = asset::Texture::Create(1280, 720, asset::texture::Format::Depth16, asset::texture::Usage::RenderTarget);
    }

    void FrameBuffer::Bind()
    {
        auto renderer = Renderer::Get<MetalRenderer>();
        MTLRenderPassDescriptor* desc = [[MTLRenderPassDescriptor alloc] init];

        MTLRenderPassColorAttachmentDescriptor* colorAttachment = [[desc colorAttachments] objectAtIndexedSubscript:0];
        const auto& colorTextureResource = static_pointer_cast<TextureResource>(m_ColorTexture->GetPlatformResource());
        [colorAttachment setTexture:colorTextureResource->GetMetalResource()];
        
        MTLRenderPassDepthAttachmentDescriptor* depthAttachment = [desc depthAttachment];
        const auto& depthTextureResource = static_pointer_cast<TextureResource>(m_DepthTexture->GetPlatformResource());
        [depthAttachment setTexture:depthTextureResource->GetMetalResource()];

        auto commandQueue = renderer->GetCommandQueue();
        m_CommandBuffer = [commandQueue commandBuffer];
        m_RenderCommandEncoder = [m_CommandBuffer renderCommandEncoderWithDescriptor:desc];
        [m_RenderCommandEncoder setCullMode:MTLCullModeBack];
        [m_RenderCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    }

    void FrameBuffer::Commit()
    {
        [m_RenderCommandEncoder endEncoding];
        [m_CommandBuffer commit];
    }
}
