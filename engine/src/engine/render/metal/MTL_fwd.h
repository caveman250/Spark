#pragma once

#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#ifdef __OBJC__
// Protocols
@protocol MTLBuffer;
@protocol MTLCommandQueue;
@protocol MTLDepthStencilState;
@protocol MTLDevice;
@protocol MTLRenderCommandEncoder;
@protocol MTLRenderPipelineState;
@protocol MTLTexture;
@protocol MTLCommandBuffer;

// Classes
@class MTLRenderPassDescriptor;
@class NSWindow;

// Protocols
typedef id<MTLBuffer> MTLBufferPtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id<MTLRenderPipelineState> RenderPipelineStatePtr;
typedef id<MTLTexture> MTLTexturePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;

// Classes
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef NSWindow* NSWindowPtr;

#else
// Protocols
typedef void* MTLBufferPtr;
typedef void* MTLCommandQueuePtr;
typedef void* MTLDepthStencilStatePtr;
typedef void* MTLDevicePtr;
typedef void* MTLRenderCommandEncoderPtr;
typedef void* RenderPipelineStatePtr;
typedef void* MTLTexturePtr;
typedef void* MTLCommandBufferPtr;

// Classes
typedef void* MTLRenderPassDescriptorPtr;
typedef void* NSWindowPtr;
#endif

#endif