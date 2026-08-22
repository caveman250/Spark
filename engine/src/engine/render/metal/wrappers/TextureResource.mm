#import "TextureResource.h"

namespace se::render::metal::wrappers
{
    MTLTexturePtr TextureResource::CreateMetalResource(MTLDevicePtr device,
        size_t width,
        size_t height,
        MTLPixelFormat pixelFormat,
        MTLTextureUsage usage)
    {
        MTLTextureDescriptorPtr textureDesc = [[MTLTextureDescriptor alloc] init];
        [textureDesc setWidth:width];
        [textureDesc setHeight:height];
        [textureDesc setPixelFormat:pixelFormat];
        [textureDesc setTextureType:MTLTextureType2D];
        [textureDesc setStorageMode:MTLStorageModeShared];
        [textureDesc setUsage:usage];


        MTLTexturePtr texture = [device newTextureWithDescriptor:textureDesc];
        [textureDesc release];
        [texture retain];
        return texture;
    }

    void TextureResource::ReplaceRegon(MTLTexturePtr texture, MTLRegion region, NSUInteger mipmapLevel, const void* withBytes, NSUInteger bytesPerRow)
    {
        [texture replaceRegion:region
                   mipmapLevel:mipmapLevel
                   withBytes:withBytes
                   bytesPerRow:bytesPerRow];
    }

    void TextureResource::Bind(MTLRenderCommandEncoderPtr commandEncoder, MTLTexturePtr texture, size_t i)
    {
        [commandEncoder setFragmentTexture:texture atIndex:i];
    }


}
