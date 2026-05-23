#pragma once 

#include "spark.h"
#include "engine/asset/mesh/Model.h"
#include "engine/ecs/Component.h"

namespace se::render::singleton_components 
{
    struct MeshRenderComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::mutex mutex = {};
        using LayerKey = std::pair<int, size_t>; //layer, baseRenderGroup
        std::map<LayerKey, size_t> layerRenderGroups = {};

#if SPARK_EDITOR
        std::vector<asset::AssetReference<asset::Model>> invalidatedMeshAssets = {};
#endif
    };
}