#pragma once

#include "RenderState.h"
#include "spark.h"
#include "VertexBuffer.h"

namespace se::render
{
    class MaterialPlatformResources
    {
    public:
        virtual ~MaterialPlatformResources() = default;
        virtual void Bind() = 0;
        virtual void CreatePlatformResources(const std::string& vert,
            const std::string& frag,
            const RenderState& rs) = 0;
        virtual void DestroyPlatformResources() = 0;
        virtual void ApplyDepthStencil(const RenderState& rs) = 0;
        virtual void ApplyBlendMode(BlendMode src, BlendMode dest) = 0;
    };
}
