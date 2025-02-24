#include "Material.h"

#include <engine/asset/shader/ast/NameGenerator.h>
#include <engine/asset/shader/ast/ShaderCompileContext.h>

#if METAL_RENDERER

#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "TextureResource.h"

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
        auto commandEncoder = renderer->GetCurrentCommandEncoder();

        commandEncoder->setRenderPipelineState(m_RenderPipelineState);

        for (size_t i = 0; i < m_Textures.size(); ++i)
        {
            m_Textures[i].second->Bind(i);
        }

        if (m_VertexUniformsSize > 0 && !m_VertexUniformBufferGpu)
        {
            auto device = renderer->GetDevice();
            m_VertexUniformBufferGpu = device->newBuffer(m_VertexUniformsSize, MTL::ResourceStorageModeManaged);
        }
        
        if (m_VertexUniformsStale)
        {
            memcpy(m_VertexUniformBufferGpu->contents(), m_VertexUniformBufferCpu, m_VertexUniformsSize);
            m_VertexUniformBufferGpu->didModifyRange(NS::Range::Make(0, m_VertexUniformBufferGpu->length()));
            m_VertexUniformsStale = false;
        }
        if (m_VertexUniformBufferGpu)
        {
            commandEncoder->setVertexBuffer(m_VertexUniformBufferGpu, 0, vb.GetVertexStreams().size());
        }

        if (m_FragmentUniformsSize > 0 && !m_FragmentUniformBufferGpu)
        {
            auto device = renderer->GetDevice();
            m_FragmentUniformBufferGpu = device->newBuffer(m_FragmentUniformsSize, MTL::ResourceStorageModeManaged);
        }
        
        if (m_FragmentUniformsStale)
        {
            memcpy(m_FragmentUniformBufferGpu->contents(), m_FragmentUniformBufferCpu, m_FragmentUniformsSize);
            m_FragmentUniformBufferGpu->didModifyRange(NS::Range::Make(0, m_FragmentUniformBufferGpu->length()));
            m_FragmentUniformsStale = false;
        }
        if (m_FragmentUniformBufferGpu)
        {
            commandEncoder->setFragmentBuffer(m_FragmentUniformBufferGpu, 0, 0);
        }
    }

    void Material::CreatePlatformResources(const render::VertexBuffer& vb)
    {
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

        debug::Log::Info("Result Frag Shader:\n {}", frag.value());

        auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

        NS::Error* error = nullptr;
        MTL::Library* vertLibrary = device->newLibrary(NS::String::string(vert->c_str(), NS::UTF8StringEncoding),
                                                       nullptr, &error);
        if (!vertLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", vert->c_str(),
                              error->localizedDescription()->utf8String());
            SPARK_ASSERT(false);
        }

        MTL::Library* fragLibrary = device->newLibrary(NS::String::string(frag->c_str(), NS::UTF8StringEncoding),
                                                       nullptr, &error);
        if (!fragLibrary)
        {
            debug::Log::Error("Failed to compile shader!\nShader Text:\n{}\nError(s):\n{}", frag->c_str(),
                              error->localizedDescription()->utf8String());
            SPARK_ASSERT(false);
        }

        MTL::Function* vertexFn = vertLibrary->newFunction(NS::String::string("vertexMain", NS::UTF8StringEncoding));
        MTL::Function* fragFn = fragLibrary->newFunction(NS::String::string("fragmentMain", NS::UTF8StringEncoding));

        MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();
        desc->setVertexFunction(vertexFn);
        desc->setFragmentFunction(fragFn);
        MTL::RenderPipelineColorAttachmentDescriptor* colorAttachment = desc->colorAttachments()->object(0);
        colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
        desc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth16Unorm);

        colorAttachment->setBlendingEnabled(m_RenderState.dstBlend != BlendMode::None);
        colorAttachment->setSourceRGBBlendFactor(MetalRenderer::BlendModeToMtl(m_RenderState.srcBlend));
        colorAttachment->setDestinationRGBBlendFactor(MetalRenderer::BlendModeToMtl(m_RenderState.dstBlend));

        NS::Error* nsError = nullptr;
        m_RenderPipelineState = device->newRenderPipelineState(desc, &nsError);
        if (!m_RenderPipelineState)
        {
            debug::Log::Error(nsError->localizedDescription()->utf8String());
            SPARK_ASSERT(false);
        }

        vertexFn->release();
        fragFn->release();
        desc->release();
        vertLibrary->release();
        fragLibrary->release();

        for (const auto& [name, uniform] : context.vertShader->GetUniformVariables())
        {
            if (!m_VertexUniformOffsets.contains(name))
            {
                m_VertexUniformOffsets[name] = m_VertexUniformsSize;
                auto varSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(uniform.type);
                if (uniform.arraySizeConstant > 0)
                {
                    varSize *= uniform.arraySizeConstant;
                }

                m_VertexUniformsSize += varSize;
            }
        }
        m_VertexUniformBufferCpu = static_cast<uint8_t*>(malloc(m_VertexUniformsSize));

        for (const auto& [name, uniform] : context.fragShader->GetUniformVariables())
        {
            if (!m_FragmentUniformOffsets.contains(name))
            {
                m_FragmentUniformOffsets[name] = m_FragmentUniformsSize;
                auto varSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(uniform.type);
                if (uniform.arraySizeConstant > 0)
                {
                    varSize *= uniform.arraySizeConstant;
                }

                m_FragmentUniformsSize += varSize;
            }
        }
        m_FragmentUniformBufferCpu = static_cast<uint8_t*>(malloc(m_FragmentUniformsSize));

        render::Material::CreatePlatformResources(vb);
    }

    void Material::DestroyPlatformResources()
    {
        m_RenderPipelineState->release();
        m_RenderPipelineState = nullptr;
        delete m_VertexUniformBufferCpu;
        m_VertexUniformBufferCpu = nullptr;
        delete m_FragmentUniformBufferCpu;
        m_FragmentUniformBufferCpu = nullptr;
        m_VertexUniformBufferGpu->release();
        m_FragmentUniformBufferGpu->release();
        render::Material::DestroyPlatformResources();
    }

    void Material::SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count,
                                      const void* value)
    {
        if (!m_PlatformResourcesCreated)
        {
            return;
        }

        switch (type)
        {
        case asset::shader::ast::AstType::Sampler2D:
        {
            SPARK_ASSERT(count == 1, "Setting arrays of texture uniforms not supported.");
            const auto& texture = *reinterpret_cast<const std::shared_ptr<asset::Texture>*>(value);
            const auto& platformResource = texture->GetPlatformResource();
            auto it = std::ranges::find_if(m_Textures, [name](const auto& kvp){ return kvp.first == name; });
            if (it != m_Textures.end())
            {
                it->second = platformResource;
            }
            else
            {
                m_Textures.push_back(std::make_pair(name, platformResource));
            }
            break;
        }
        case asset::shader::ast::AstType::Invalid:
        {
            debug::Log::Error("Material::SetUniform - Unhandled uniform type {}", asset::shader::ast::TypeUtil::TypeToMtl(type));
            break;
        }
        default:
        {
            if (m_VertexUniformOffsets.contains(name))
            {
                for (int i = 0; i < count; ++i)
                {
                    size_t typeSize = asset::shader::ast::TypeUtil::GetTypeSize(type);
                    size_t paddedSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(type);
                    size_t padding = paddedSize - typeSize;
                    size_t offset = m_VertexUniformOffsets[name] + paddedSize * i;
                    memcpy(m_VertexUniformBufferCpu + offset, (uint8_t*)value + typeSize * i, asset::shader::ast::TypeUtil::GetTypeSize(type));
                    if (padding > 0)
                    {
                        memset(m_VertexUniformBufferCpu + offset + typeSize, 0, padding);
                    }
                }
                
                m_VertexUniformsStale = true;
            }
            if (m_FragmentUniformOffsets.contains(name))
            {
                for (int i = 0; i < count; ++i)
                {
                    
                    size_t typeSize = asset::shader::ast::TypeUtil::GetTypeSize(type);
                    size_t paddedSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(type);
                    size_t padding = paddedSize - typeSize;
                    size_t offset = m_FragmentUniformOffsets[name] + paddedSize * i;
                    memcpy(m_FragmentUniformBufferCpu + offset, (uint8_t*)value + typeSize * i, asset::shader::ast::TypeUtil::GetTypeSize(type));
                    if (padding > 0)
                    {
                        memset(m_FragmentUniformBufferCpu + offset + typeSize, 0, padding);
                    }
                }
                
                m_FragmentUniformsStale = true;
            }

            break;
        }
        }
    }

    void Material::ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask)
    {
        if (!m_DepthStencilState)
        {
            auto device = Renderer::Get<metal::MetalRenderer>()->GetDevice();

            MTL::DepthStencilDescriptor* depthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
            depthStencilDesc->setDepthCompareFunction(MetalRenderer::DepthCompareToMtl(m_RenderState.depthComp));
            depthStencilDesc->setDepthWriteEnabled(m_RenderState.depthComp != DepthCompare::None);
            m_DepthStencilState = device->newDepthStencilState(depthStencilDesc);
            depthStencilDesc->release();
        }

        auto commandEncoder = Renderer::Get<MetalRenderer>()->GetCurrentCommandEncoder();
        commandEncoder->setDepthStencilState(m_DepthStencilState);
    }

    void Material::ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest)
    {
        // nothing to do here. Has to be done as part of the pipeline state creation.
    }
}

#endif
