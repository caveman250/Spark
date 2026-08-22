module;

#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "wrappers/TextureResource.h"

#include "engine/render/metal/MTL_fwd.h"
#include "wrappers/RenderCommand.h"

module Spark.Render.Metal.TextureResource;

import Spark.Render.Renderer;
import Spark.Render.Metal.MetalRenderer;

MTLPixelFormat TextureFormatToMetalFormat(se::asset::texture::Format format)
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

MTLTextureUsage TextureUsageToMetalUsage(se::asset::texture::Usage format)
{
    switch (format)
    {
        case se::asset::texture::Usage::Read:
            return MTLTextureUsageShaderRead;
        case se::asset::texture::Usage::RenderTarget:
            return MTLTextureUsageRenderTarget;
        case se::asset::texture::Usage::Read_RenderTarget:
            return MTLTextureUsage(MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget);
        default:
            SPARK_ASSERT(false, "TextureUsageToMetalUsage - Unhandled usage");
            return MTLTextureUsageUnknown;
    }
}

namespace se::render::metal
{
    void TextureResource::CreatePlatformResources()
    {
        auto renderer = Renderer::Get<MetalRenderer>();
        wrappers::TextureResource::CreateMetalResource(renderer->GetDevice(),
            m_Width,
            m_Height,
            TextureFormatToMetalFormat(m_Format),
            TextureUsageToMetalUsage(m_Usage));

        size_t bytesPerRow = 0;
        if (IsCompressedFormat(m_Format))
        {
            size_t blockSize = GetCompressedFormatBlockSize(m_Format);
            size_t bytesPerBlock = GetCompressedFormatBlockSizeBytes(m_Format);
            size_t blocksPerRow = (m_Width + (blockSize - 1)) / blockSize;
            bytesPerRow = blocksPerRow * bytesPerBlock;
        }
        else
        {
            bytesPerRow = m_Width * GetNumTextureChannels(m_Format);
        }

        if (!m_MipData.empty())
        {
            wrappers::TextureResource::ReplaceRegon(m_MetalResource,
                { 0, 0, 0, m_Width, m_Height, 1 },
                0,
                (const void *)m_MipData[0].m_Data.GetData(),
                bytesPerRow);
        }

        render::TextureResource::CreatePlatformResources();
    }

    void TextureResource::Bind(size_t i)
    {
        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        wrappers::TextureResource::Bind(renderer->GetCurrentCommandEncoder(), m_MetalResource, i);
    }
}
#endif
