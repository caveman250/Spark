#include "TextureResource.h"
#include "MetalRenderer.h"

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
        MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::alloc()->init();
        textureDesc->setWidth(m_Texture.GetWidth());
        textureDesc->setHeight(m_Texture.GetHeight());
        textureDesc->setPixelFormat(TextureFormatToMetalFormat(m_Texture.GetFormat()));
        textureDesc->setTextureType(MTL::TextureType2D);
        textureDesc->setStorageMode(MTL::StorageModeManaged);
        textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

        m_MetalResource = Renderer::Get<MetalRenderer>()->GetDevice()->newTexture(textureDesc);
        
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
        m_MetalResource->replaceRegion(MTL::Region(0, 0, 0, m_Texture.GetWidth(), m_Texture.GetHeight(), 1),
                                       0,
                                       m_Texture.GetMips()[0].m_Data.GetData(),
                                       bytesPerRow);
            

        textureDesc->release();
        render::TextureResource::CreatePlatformResources();
    }

    void TextureResource::Bind(size_t i)
    {
        auto renderer = Renderer::Get<MetalRenderer>();
        renderer->GetCurrentCommandEncoder()->setFragmentTexture(m_MetalResource, i);
    }

    MTL::PixelFormat TextureResource::TextureFormatToMetalFormat(asset::texture::Format::Type format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return MTL::PixelFormatR8Unorm;
            case asset::texture::Format::BC7:
                return MTL::PixelFormatBC7_RGBAUnorm;
            case asset::texture::Format::DXT5:
                SPARK_ASSERT(false, "DXT5 not supported in Metal");
                return MTL::PixelFormatInvalid;
            default:
                return MTL::PixelFormatInvalid;
        }
    }
}
#endif
