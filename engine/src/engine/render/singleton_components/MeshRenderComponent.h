#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::render::singleton_components 
{
    struct MeshRenderComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::mutex mutex = {};
        std::map<int, size_t> layerRenderGroups = {};
    };
}