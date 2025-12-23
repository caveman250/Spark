#pragma once
#include "engine/render/render_fwd.h"
#include "MTL_fwd.h"
#if METAL_RENDERER

#include "engine/asset/texture/Texture.h"
#include "engine/render/MaterialInstance.h"

namespace se::render::metal
{
    class MaterialInstance : public render::MaterialInstance
    {
    public:
        explicit MaterialInstance(const std::shared_ptr<render::Material>& material);
        void Bind(const render::VertexBuffer&) override;
        void CreatePlatformResources() override;
        void DestroyPlatformResources() override;

    private:
        void SetUniformInternal(const std::string& name, asset::shader::ast::AstType type, int count, const void* value) override;

        std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>> m_Textures;

        bool m_VertexUniformsStale = false;
        size_t m_VertexUniformsSize = 0;
        std::unordered_map<std::string, size_t> m_VertexUniformOffsets;
        uint8_t* m_VertexUniformBufferCpu = nullptr;
        MTLBufferPtr m_VertexUniformBufferGpu = nullptr;

        bool m_FragmentUniformsStale = false;
        size_t m_FragmentUniformsSize = 0;
        std::unordered_map<std::string, size_t> m_FragmentUniformOffsets;
        uint8_t* m_FragmentUniformBufferCpu = nullptr;
        MTLBufferPtr m_FragmentUniformBufferGpu = nullptr;
    };
}
#endif
