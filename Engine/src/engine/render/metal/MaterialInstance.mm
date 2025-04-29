#include "MaterialInstance.h"

#if METAL_RENDERER

#include "TextureResource.h"
#import <Metal/Metal.h>

namespace se::render
{
    std::shared_ptr<MaterialInstance> MaterialInstance::CreateMaterialInstance(const std::shared_ptr<render::Material>& material)
    {
        SPARK_ASSERT(material);
        return std::make_shared<metal::MaterialInstance>(material);
    }
}

namespace se::render::metal
{
    MaterialInstance::MaterialInstance(const std::shared_ptr<render::Material>& material)
        : render::MaterialInstance(material)
    {
    }

    void MaterialInstance::Bind(const render::VertexBuffer& vb)
    {
        render::MaterialInstance::Bind(vb);

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

    void MaterialInstance::CreatePlatformResources()
    {
        PROFILE_SCOPE("MaterialInstance::CreatePlatformResources")

        auto vertUniforms = m_Material->GetVertUniforms();
        for (const auto& [name, uniform] : vertUniforms)
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

        auto fragUniforms = m_Material->GetFragUniforms();
        for (const auto& [name, uniform] : fragUniforms)
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

        render::MaterialInstance::CreatePlatformResources();
    }

    void MaterialInstance::DestroyPlatformResources()
    {
        delete m_VertexUniformBufferCpu;
        m_VertexUniformBufferCpu = nullptr;
        delete m_FragmentUniformBufferCpu;
        m_FragmentUniformBufferCpu = nullptr;
        [m_VertexUniformBufferGpu release];
        [m_FragmentUniformBufferGpu release];
        render::MaterialInstance::DestroyPlatformResources();
    }

    void MaterialInstance::SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count,
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
