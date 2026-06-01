#include "MaterialInstancePlatformResources.h"
#include "MaterialPlatformResources.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "engine/render/MaterialInstance.h"

#if METAL_RENDERER

namespace se::render
{
    std::shared_ptr<MaterialInstancePlatformResources> MaterialInstance::CreateMaterialInstancePlatformResources(const std::shared_ptr<Material>& material)
    {
        auto platformResources = std::make_shared<metal::MaterialInstancePlatformResources>();
        platformResources->CreatePlatformResources(material);
        return platformResources;
    }
}

namespace se::render::metal
{
    void MaterialInstancePlatformResources::Bind(const render::VertexBuffer& vb)
    {
        auto renderer = Renderer::Get<MetalRenderer>();
        auto commandEncoder = (id<MTLRenderCommandEncoder>)renderer->GetCurrentCommandEncoder();
        auto device = (id<MTLDevice>)renderer->GetDevice();

        for (size_t i = 0; i < m_Textures.size(); ++i)
        {
            m_Textures[i].second->Bind(i);
        }

        if (m_VertexUniformsSize > 0 && !m_VertexUniformBufferGpu)
        {
            m_VertexUniformBufferGpu = [device newBufferWithLength:m_VertexUniformsSize options:MTLStorageModeShared];
        }

        if (m_VertexUniformsStale)
        {
            memcpy([m_VertexUniformBufferGpu contents], m_VertexUniformBufferCpu, m_VertexUniformsSize);
            m_VertexUniformsStale = false;
        }
        if (m_VertexUniformBufferGpu)
        {
            [commandEncoder setVertexBuffer:m_VertexUniformBufferGpu offset:0 atIndex:vb.GetVertexStreams().size()];
        }

        if (m_FragmentUniformsSize > 0 && !m_FragmentUniformBufferGpu)
        {
            m_FragmentUniformBufferGpu = [device newBufferWithLength:m_FragmentUniformsSize options:MTLStorageModeShared];
        }

        if (m_FragmentUniformsStale)
        {
            memcpy([m_FragmentUniformBufferGpu contents], m_FragmentUniformBufferCpu, m_FragmentUniformsSize);
            m_FragmentUniformsStale = false;
        }
        if (m_FragmentUniformBufferGpu)
        {
            [commandEncoder setFragmentBuffer:m_FragmentUniformBufferGpu offset:0 atIndex:0];
        }
    }

    void MaterialInstancePlatformResources::CreatePlatformResources(const std::shared_ptr<Material>& material)
    {
        EASY_FUNCTION();

        auto vertUniforms = material->GetVertUniforms();
        for (const auto& [name, uniform] : vertUniforms)
        {
            if (!m_VertexUniformOffsets.contains(name))
            {
                auto varSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(uniform.var.type);
                auto alignment = asset::shader::ast::TypeUtil::GetTypeMetalAlignment(uniform.var.type);
                int remainder = m_VertexUniformsSize % alignment;
                if (remainder > 0)
                {
                    m_VertexUniformsSize += (alignment - remainder);
                }
                m_VertexUniformOffsets[name] = m_VertexUniformsSize;
                if (uniform.var.arraySizeConstant > 0)
                {
                    varSize *= uniform.var.arraySizeConstant;
                }

                m_VertexUniformsSize += varSize;
            }

        }
        m_VertexUniformBufferCpu = static_cast<uint8_t*>(malloc(m_VertexUniformsSize));

        auto fragUniforms = material->GetFragUniforms();
        for (const auto& [name, uniform] : fragUniforms)
        {
            if (!m_FragmentUniformOffsets.contains(name))
            {
                auto varSize = asset::shader::ast::TypeUtil::GetTypePaddedSize(uniform.var.type);
                auto alignment = asset::shader::ast::TypeUtil::GetTypeMetalAlignment(uniform.var.type);
                int remainder = m_FragmentUniformsSize % alignment;
                if (remainder > 0)
                {
                    m_FragmentUniformsSize += (alignment - remainder);
                }
                m_FragmentUniformOffsets[name] = m_FragmentUniformsSize;
                if (uniform.var.arraySizeConstant > 0)
                {
                    varSize *= uniform.var.arraySizeConstant;
                }

                m_FragmentUniformsSize += varSize;
            }
        }
        m_FragmentUniformBufferCpu = static_cast<uint8_t*>(malloc(m_FragmentUniformsSize));
    }

    void MaterialInstancePlatformResources::DestroyPlatformResources()
    {
        delete m_VertexUniformBufferCpu;
        m_VertexUniformBufferCpu = nullptr;
        delete m_FragmentUniformBufferCpu;
        m_FragmentUniformBufferCpu = nullptr;
        [m_VertexUniformBufferGpu release];
        [m_FragmentUniformBufferGpu release];
    }

    void MaterialInstancePlatformResources::SetUniformInternal(const std::string& name,
                                                                                   asset::shader::ast::AstType type,
                                                                                   int count,
                                                                                   const void* value,
                                                                                   const std::shared_ptr<Material>&)
    {
        switch (type)
        {
        case asset::shader::ast::AstType::Sampler2D:
        {
            SPARK_ASSERT(count == 1, "Setting arrays of texture uniforms not supported.");
            const auto& texture = *reinterpret_cast<const std::shared_ptr<asset::Texture>*>(value);
            if (!texture)
            {
                return;
            }

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
        case asset::shader::ast::AstType::Sampler2DReference:
        {
            SPARK_ASSERT(count == 1, "Setting arrays of texture uniforms not supported.");
            const asset::AssetReference<asset::Texture>* textureRef = static_cast<const asset::AssetReference<asset::Texture>*>(value);
            if (!textureRef || !textureRef->IsSet())
            {
                return;
            }

            auto texture = textureRef->GetAsset();
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
            debug::Log::Error("MaterialInstance::SetUniform - Unhandled uniform type {}", asset::shader::ast::TypeUtil::TypeToMtl(type));
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
                    memcpy(m_FragmentUniformBufferCpu + offset, (uint8_t*)value + typeSize * i, typeSize);
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
}

#endif