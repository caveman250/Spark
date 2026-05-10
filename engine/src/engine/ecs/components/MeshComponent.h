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
        asset::AssetReference<render::Material> materialAsset = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<render::MaterialInstance> materialInstanceAsset = {};

        SPARK_MEMBER(Serialized)
        int renderLayer = 0;

        SPARK_MEMBER(Serialized)
        bool visible = true;

        // Runtime
        SPARK_MEMBER()
        std::shared_ptr<render::MaterialInstance> materialInstance = {};

        std::shared_ptr<render::VertexBuffer> vertexBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        geo::AABB aabb = {};
    };
}
