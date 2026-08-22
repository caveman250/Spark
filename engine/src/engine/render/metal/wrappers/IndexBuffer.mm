#import "IndexBuffer.h"

namespace se::render::metal::wrappers
{
    MTLBufferPtr IndexBuffer::CreatePlatfromResources(MTLDevicePtr device, size_t size, uint32_t* data)
    {
        MTLBufferPtr buffer = [device newBufferWithLength:size options:MTLStorageModeShared];
        memcpy([buffer contents], data, size);
        [buffer retain];

        return buffer;
    }

    void IndexBuffer::Release(MTLBufferPtr buffer)
    {
        [buffer release];
    }
}