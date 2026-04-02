#pragma once

#include "spark.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/Material.h"

namespace se::render
{
    class MaterialInstancePlatformResources
    {
    public:
        virtual ~MaterialInstancePlatformResources() = default;
        virtual void Bind(const VertexBuffer& vb) = 0;
        virtual void CreatePlatformResources(const std::shared_ptr<Material>& material) = 0;
        virtual void DestroyPlatformResources() = 0;
        virtual void SetUniformInternal(const std::string& name,
                                asset::shader::ast::AstType type,
                                int count,
                                const void* value,
                                const std::shared_ptr<Material>& material) = 0;
    };
}
