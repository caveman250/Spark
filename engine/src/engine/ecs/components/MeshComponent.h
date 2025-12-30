#pragma once

#include "spark.h"
#include "engine/asset/AssetReference.h"
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
        SPARK_COMPONENT()

        MeshComponent() = default;

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Model> model = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<render::Material> material = {};

        // Runtime
        SPARK_MEMBER()
        std::shared_ptr<render::MaterialInstance> materialInstance = {};

        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
    };
}
