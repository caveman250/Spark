#import "RenderCommand.h"

#if METAL_RENDERER

namespace se::render::metal::wrappers
{
    void RenderCommand::RenderGeo(MTLRenderCommandEncoderPtr commandEncoder,
        MTLPrimitiveType primType,
        MTLBufferPtr vertexBuffer,
        size_t numVertices)
    {
        [commandEncoder setVertexBuffer:vertexBuffer
              offset:0
             atIndex:0];

        [commandEncoder drawPrimitives:primType
                    vertexStart:0
                    vertexCount:numVertices];
    }

    void RenderCommand::RenderIndexedGeo(MTLRenderCommandEncoderPtr commandEncoder,
        MTLPrimitiveType primType,
        MTLBufferPtr indexBuffer,
        size_t numIndices)
    {
        [commandEncoder drawIndexedPrimitives:primType
                           indexCount:numIndices
                           indexType:MTLIndexTypeUInt32
                           indexBuffer:indexBuffer
                           indexBufferOffset:0
                           instanceCount:1];
    }

    void RenderCommand::SetScissorRect(MTLRenderCommandEncoderPtr commandEncoder, MTLScissorRect scissor)
    {
        [commandEncoder setScissorRect:scissor];
    }
}

#endif