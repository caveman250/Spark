#include "Material.h"

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
    std::shared_ptr<Material> Material::CreateMaterial(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
    {
        return std::make_shared<metal::Material>(vertShaders, fragShaders);
    }
}

namespace se::render::metal
{
    Material::Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : render::Material(vertShaders, fragShaders)
    {
    }

    void Material::Bind(const render::VertexBuffer& vb)
    {
        render::Material::Bind(vb);

        auto renderer = Renderer::Get<MetalRenderer>();
        auto boundMat = renderer->GetBoundMaterial();
        if (boundMat == this)
        {
            return;
        }

        renderer->SetBoundMaterial(this);

        auto commandEncoder = (id<MTLRenderCommandEncoder>)renderer->GetCurrentCommandEncoder();
        [commandEncoder setRenderPipelineState:m_RenderPipelineState];
    }

    void Material::CreatePlatformResources(const render::VertexBuffer& vb)
    {
        PROFILE_SCOPE("Material::CreatePlatformResources")
        asset::shader::ast::ShaderCompileContext context = {
            nullptr, nullptr, nullptr, asset::shader::ast::NameGenerator::GetName(), {}, {}
        };

        auto fragUniforms = asset::shader::ShaderCompiler::GatherUsedUniforms(m_FragShaders);
        context.fragmentShaderUniforms = fragUniforms;
        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(
            m_VertShaders, m_ShaderSettings, vb, context);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(
            m_FragShaders, m_ShaderSettings, vb, context);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }

        m_VertUniforms = context.vertShader->GetUniformVariables();
        m_FragUniforms = context.fragShader->GetUniformVariables();

        SPARK_ASSERT(!m_VertUniforms.empty());

        auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

        NSError* error = nullptr;
        id<MTLLibrary> vertLibrary = [device newLibraryWithSource:[[NSString alloc] initWithUTF8String:vert->c_str()]
                                            options:nullptr
                                            error:&error];
        if (!vertLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", vert->c_str(), [[error localizedDescription] UTF8String]);
            SPARK_ASSERT(false);
        }

        id<MTLLibrary> fragLibrary = [device newLibraryWithSource:[[NSString alloc] initWithUTF8String:frag->c_str()]
                                                          options:nullptr
                                                            error:&error];
        if (!fragLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", frag->c_str(), [[error localizedDescription] UTF8String]);
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

        [colorAttachment setBlendingEnabled:m_RenderState.dstBlend != BlendMode::None];
        [colorAttachment setSourceRGBBlendFactor:BlendModeToMtl(m_RenderState.srcBlend)];
        [colorAttachment setDestinationRGBBlendFactor:BlendModeToMtl(m_RenderState.dstBlend)];

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

        render::Material::CreatePlatformResources(vb);
    }

    void Material::DestroyPlatformResources()
    {
        [m_RenderPipelineState release];
        m_RenderPipelineState = nullptr;
        render::Material::DestroyPlatformResources();
    }

    void Material::ApplyDepthStencil(DepthCompare::Type, StencilFunc::Type, uint32_t, uint32_t)
    {
        if (!m_DepthStencilState)
        {
            auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

            MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
            [depthStencilDesc setDepthCompareFunction:DepthCompareToMtl(m_RenderState.depthComp)];
            [depthStencilDesc setDepthWriteEnabled:m_RenderState.depthComp != DepthCompare::None];
            m_DepthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDesc];
            [depthStencilDesc release];
        }

        auto commandEncoder = (id<MTLRenderCommandEncoder>)Renderer::Get<MetalRenderer>()->GetCurrentCommandEncoder();
        [commandEncoder setDepthStencilState:m_DepthStencilState];
    }

    void Material::ApplyBlendMode(BlendMode::Type, BlendMode::Type)
    {
        // nothing to do here. Has to be done as part of the pipeline state creation.
    }
}

#endif
