#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/asset/texture/Texture.h"
#include "engine/render/Material.h"

namespace se::render::metal
{
    class Material : public render::Material
    {
    public:
        Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                 const std::vector<std::shared_ptr<asset::Shader>>& fragShaders);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources(const render::VertexBuffer& vb) override;
        void DestroyPlatformResources() override;

    private:
        void SetUniformInternal(const std::string& name, asset::shader::ast::AstType::Type type, int count, const void* value) override;
        void ApplyDepthStencil(DepthCompare::Type comp, StencilFunc::Type src, uint32_t writeMask, uint32_t readMask) override;
        void ApplyBlendMode(BlendMode::Type src, BlendMode::Type dest) override;

        MTL::RenderPipelineState* m_RenderPipelineState = nullptr;
        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> m_Textures;

        bool m_VertexUniformsStale = false;
        size_t m_VertexUniformsSize = 0;
        std::unordered_map<std::string, size_t> m_VertexUniformOffsets;
        uint8_t* m_VertexUniformBufferCpu = nullptr;
        MTL::Buffer* m_VertexUniformBufferGpu = nullptr;

        bool m_FragmentUniformsStale = false;
        size_t m_FragmentUniformsSize = 0;
        std::unordered_map<std::string, size_t> m_FragmentUniformOffsets;
        uint8_t* m_FragmentUniformBufferCpu = nullptr;
        MTL::Buffer* m_FragmentUniformBufferGpu = nullptr;

        MTL::DepthStencilState* m_DepthStencilState = nullptr;
    };
}
#endif
