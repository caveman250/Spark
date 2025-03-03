#import "TextureResource.h"
#import "engine/render/metal/MetalRenderer.h"
#import <Metal/Metal.h>

MTLPixelFormat TextureFormatToMetalFormat(se::asset::texture::Format::Type format)
{
    switch (format)
    {
        case se::asset::texture::Format::R8:
            return MTLPixelFormatR8Unorm;
        case se::asset::texture::Format::BC7:
            return MTLPixelFormatBC7_RGBAUnorm;
        case se::asset::texture::Format::DXT5:
            SPARK_ASSERT(false, "DXT5 not supported in Metal");
            return MTLPixelFormatInvalid;
        default:
            return MTLPixelFormatInvalid;
    }
}

#if METAL_RENDERER

namespace se::render
{
    std::shared_ptr<TextureResource> TextureResource::Create(const asset::Texture& texture)
    {
        return std::make_shared<metal::TextureResource>(texture);
    }
}

namespace se::render::metal
{
    void TextureResource::CreatePlatformResources()
    {
        MTLTextureDescriptor* textureDesc = [[MTLTextureDescriptor alloc] init];
        [textureDesc setWidth:m_Texture.GetWidth()];
        [textureDesc setHeight:m_Texture.GetHeight()];
        [textureDesc setPixelFormat:TextureFormatToMetalFormat(m_Texture.GetFormat())];
        [textureDesc setTextureType:MTLTextureType2D];
        [textureDesc setStorageMode:MTLStorageModeManaged];
        [textureDesc setUsage:MTLResourceUsageRead];

        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        m_MetalResource = [renderer->GetDevice() newTextureWithDescriptor:textureDesc];

        size_t bytesPerRow = 0;
        if (IsCompressedFormat(m_Texture.GetFormat()))
        {
            size_t blockSize = GetCompressedFormatBlockSize(m_Texture.GetFormat());
            size_t bytesPerBlock = GetCompressedFormatBlockSizeBytes(m_Texture.GetFormat());
            int blocksPerRow = (m_Texture.GetWidth() + (blockSize - 1)) / blockSize;
            bytesPerRow = blocksPerRow * bytesPerBlock;
        }
        else
        {
            bytesPerRow = m_Texture.GetWidth() * GetNumTextureChannels(m_Texture.GetFormat());
        }


        [m_MetalResource replaceRegion:MTLRegionMake2D(0, 0, m_Texture.GetWidth(), m_Texture.GetHeight())
                         mipmapLevel: 0
                           withBytes: (const void*)m_Texture.GetMips()[0].m_Data.GetData()
                         bytesPerRow:bytesPerRow];


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