#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/asset/mesh/Model.h"

namespace se::render
{
    class MaterialInstance;
    class VertexBuffer;
    class IndexBuffer;
}

namespace se::ecs::components
{
    struct MeshComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(MeshComponent)

        MeshComponent();

        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        std::shared_ptr<asset::Model> model = {};
    };
}
