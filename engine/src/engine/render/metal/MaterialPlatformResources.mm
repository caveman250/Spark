#include "MaterialPlatformResources.h"

#include <engine/asset/shader/ast/NameGenerator.h>
#include <engine/asset/shader/ast/ShaderCompileContext.h>

#if METAL_RENDERER

#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "TextureResource.h"
#import <Metal/Metal.h>
#import "engine/render/metal/MetalTypeUtil.h"

namespace se::render
{
    std::shared_ptr<MaterialPlatformResources> Material::CreateMaterialPlatformResources()
    {
        return std::make_shared<metal::MaterialPlatformResources>();
    }
}

namespace se::render::metal
{
    void MaterialPlatformResources::Bind()
    {
        auto renderer = Renderer::Get<MetalRenderer>();
        auto commandEncoder = (id<MTLRenderCommandEncoder>)renderer->GetCurrentCommandEncoder();
        [commandEncoder setRenderPipelineState:m_RenderPipelineState];
    }

    void MaterialPlatformResources::CreatePlatformResources(const std::string& vert,
                                                            const std::string& frag,
                                                            const render::RenderState& rs)
    {
        EASY_FUNCTION();

        auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

        NSError* error = nullptr;
        id<MTLLibrary> vertLibrary = [device newLibraryWithSource:[[NSString alloc] initWithUTF8String:vert.c_str()]
                                            options:nullptr
                                            error:&error];
        if (!vertLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", vert.c_str(), [[error localizedDescription] UTF8String]);
            SPARK_ASSERT(false);
        }

        id<MTLLibrary> fragLibrary = [device newLibraryWithSource:[[NSString alloc] initWithUTF8String:frag.c_str()]
                                                          options:nullptr
                                                            error:&error];
        if (!fragLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", frag.c_str(), [[error localizedDescription] UTF8String]);
            SPARK_ASSERT(false);
        }

        id<MTLFunction> vertexFn = [vertLibrary newFunctionWithName:[[NSString alloc] initWithUTF8String:"vertexMain"]];
        id<MTLFunction> fragFn = [fragLibrary newFunctionWithName:[[NSString alloc] initWithUTF8String:"fragmentMain"]];

        MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
        [desc setVertexFunction:vertexFn];
        [desc setFragmentFunction:fragFn];

        MTLRenderPipelineColorAttachmentDescriptor* colorAttachment = [[desc colorAttachments] objectAtIndexedSubscript:0];
        [colorAttachment setPixelFormat:MTLPixelFormatBGRA8Unorm];
        [desc setDepthAttachmentPixelFormat:MTLPixelFormatDepth16Unorm];

        [colorAttachment setBlendingEnabled:rs.dstBlend != BlendMode::None];
        [colorAttachment setSourceRGBBlendFactor:BlendModeToMtl(rs.srcBlend)];
        [colorAttachment setDestinationRGBBlendFactor:BlendModeToMtl(rs.dstBlend)];

        m_RenderPipelineState = [device newRenderPipelineStateWithDescriptor:desc error:&error];
        if (!m_RenderPipelineState)
        {
            debug::Log::Error([[error localizedDescription] UTF8String]);
            SPARK_ASSERT(false);
        }

        [vertexFn release];
        [fragFn release];
        [desc release];
        [vertLibrary release];
        [fragLibrary release];
    }

    void MaterialPlatformResources::DestroyPlatformResources()
    {
        [m_RenderPipelineState release];
        m_RenderPipelineState = nullptr;
    }

    void MaterialPlatformResources::ApplyDepthStencil(const render::RenderState& rs)
    {
        if (!m_DepthStencilState)
        {
            auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

            MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
            [depthStencilDesc setDepthCompareFunction:DepthCompareToMtl(rs.depthComp)];
            [depthStencilDesc setDepthWriteEnabled:rs.depthComp != DepthCompare::None];
            m_DepthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDesc];
            [depthStencilDesc release];
        }

        auto commandEncoder = (id<MTLRenderCommandEncoder>)Renderer::Get<MetalRenderer>()->GetCurrentCommandEncoder();
        [commandEncoder setDepthStencilState:m_DepthStencilState];
    }

    void MaterialPlatformResources::ApplyBlendMode(BlendMode, BlendMode)
    {
        // nothing to do here. Has to be done as part of the pipeline state creation.
    }
}

#endif
