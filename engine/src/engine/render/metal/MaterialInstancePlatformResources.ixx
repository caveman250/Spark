module;

#include "engine/render/render_fwd.h"
#include "MTL_fwd.h"
#if METAL_RENDERER

export module Spark.Render.Metal.MaterialInstancePlatformResources;
import Spark.Render.VertexBuffer;
import Spark.Render.TextureResource;

namespace se::render::metal
{
    export class MaterialInstancePlatformResources : public render::MaterialInstancePlatformResources
    {
    public:
        void Bind(const render::VertexBuffer& vb) override;
        void CreatePlatformResources(const std::shared_ptr<Material>& material) override;
        void DestroyPlatformResources() override;
        void SetUniformInternal(const std::string& name,
            asset::shader::ast::AstType type,
            int count,
            const void* value,
            const std::shared_ptr<Material>& material) override;

    private:
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