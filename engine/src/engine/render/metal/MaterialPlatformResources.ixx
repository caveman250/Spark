module;

#include <string>

#if METAL_RENDERER
#include "engine/render/metal/MTL_fwd.h"

export module Spark.Render.Metal.MaterialPlatformResources;
import Spark.Render.MaterialPlatformResources;
import Spark.Render.RenderState;

namespace se::render::metal
{
    class MaterialPlatformResources : public render::MaterialPlatformResources
    {
    public:
        void Bind() override;
        void CreatePlatformResources(const std::string& vert,
            const std::string& frag,
            const RenderState& rs) override;
        void DestroyPlatformResources() override;

    private:
        void ApplyDepthStencil(const RenderState& rs) override;
        void ApplyBlendMode(BlendMode src, BlendMode dest) override;
        void ApplyCullMode(CullMode cull) override;

        RenderPipelineStatePtr m_RenderPipelineState = nullptr;
        MTLDepthStencilStatePtr m_DepthStencilState = nullptr;
    };
}
#endif
