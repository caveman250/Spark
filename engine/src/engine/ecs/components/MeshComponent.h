#pragma once

#include "spark.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/mesh/Model.h"
#include "engine/ecs/Component.h"
#include "engine/render/MaterialInstance.h"

namespace se::render
{
    class MaterialInstance;
    class VertexBuffer;
    class IndexBuffer;
}

namespace se::ecs::components
{
    struct MeshComponent : Component
    {
        SPARK_COMPONENT()

        MeshComponent() = default;

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Model> model = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<render::Material> material = {};

        SPARK_MEMBER(Serialized)
        int renderLayer = 0;

        // Runtime
        SPARK_MEMBER()
        std::shared_ptr<render::MaterialInstance> materialInstance = {};

        size_t modelHash = 0;
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
    };
}
