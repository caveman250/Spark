#import "TextureResource.h"
#import "engine/render/metal/MetalRenderer.h"
#include "engine/render/TextureResource.h"

#if METAL_RENDERER

#import <Metal/Metal.h>

MTLPixelFormat TextureFormatToMetalFormat(se::asset::texture::Format::Type format)
{
    switch (format)
    {
        case se::asset::texture::Format::R8:
            return MTLPixelFormatR8Unorm;
        case se::asset::texture::Format::BGRA8:
            return MTLPixelFormatBGRA8Unorm;
        case se::asset::texture::Format::RGBA8:
            return MTLPixelFormatRGBA8Unorm;
        case se::asset::texture::Format::Depth16:
            return MTLPixelFormatDepth16Unorm;
        case se::asset::texture::Format::BC7:
            return MTLPixelFormatBC7_RGBAUnorm;
        case se::asset::texture::Format::DXT5:
            SPARK_ASSERT(false, "DXT5 not supported in Metal");
            return MTLPixelFormatInvalid;
        default:
            SPARK_ASSERT(false, "TextureFormatToMetalFormat - Unhandled format");
            return MTLPixelFormatInvalid;
    }
}

MTLTextureUsage TextureUsageToMetalUsage(se::asset::texture::Usage::Type format)
{
    switch (format)
    {
        case se::asset::texture::Usage::Read:
            return MTLTextureUsageShaderRead;
        case se::asset::texture::Usage::RenderTarget:
            return MTLTextureUsageRenderTarget;
        case se::asset::texture::Usage::Read_RenderTarget:
            return MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
        default:
            SPARK_ASSERT(false, "TextureUsageToMetalUsage - Unhandled usage");
            return MTLTextureUsageUnknown;
    }
}

namespace se::render
{
    std::shared_ptr<TextureResource> TextureResource::Create(const asset::Texture &texture)
    {
        return std::make_shared<metal::TextureResource>(texture);
    }
}

namespace se::render::metal
{
    void TextureResource::CreatePlatformResources()
    {
        MTLTextureDescriptor *textureDesc = [[MTLTextureDescriptor alloc] init];
        [textureDesc setWidth:m_Width];
        [textureDesc setHeight:m_Height];
        [textureDesc setPixelFormat:TextureFormatToMetalFormat(m_Format)];
        [textureDesc setTextureType:MTLTextureType2D];
        [textureDesc setStorageMode:MTLStorageModeShared];
        [textureDesc setUsage:TextureUsageToMetalUsage(m_Usage)];

        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        m_MetalResource = [renderer->GetDevice() newTextureWithDescriptor:textureDesc];

        size_t bytesPerRow = 0;
        if (IsCompressedFormat(m_Format))
        {
            size_t blockSize = GetCompressedFormatBlockSize(m_Format);
            size_t bytesPerBlock = GetCompressedFormatBlockSizeBytes(m_Format);
            int blocksPerRow = (m_Width + (blockSize - 1)) / blockSize;
            bytesPerRow = blocksPerRow * bytesPerBlock;
        }
        else
        {
            bytesPerRow = m_Width * GetNumTextureChannels(m_Format);
        }


        if (!m_MipData.empty())
        {
            [m_MetalResource replaceRegion:MTLRegionMake2D(0, 0, m_Width, m_Height)
                               mipmapLevel:0
                                 withBytes:(const void *)m_MipData[0].m_Data.GetData()
                               bytesPerRow:bytesPerRow];
        }



        [textureDesc release];
        [m_MetalResource retain];

        render::TextureResource::CreatePlatformResources();
    }

    void TextureResource::Bind(size_t i)
    {
        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        [renderer->GetCurrentCommandEncoder() setFragmentTexture:m_MetalResource atIndex:i];
    }
}
#endif
