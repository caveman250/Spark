module;

#include "spark.h"

export module Spark.Render.MaterialInstancePlatformResources;
import Spark.Render.VertexBuffer;
import Material;
import Spark.Asset.Shader;

namespace se::render
{
    export class MaterialInstancePlatformResources
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
