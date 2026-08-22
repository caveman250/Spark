#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::asset
{
    class Asset;
}

namespace se::editor::singleton_components
{
    struct DragDropStateComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT(EditorOnly)

        std::shared_ptr<asset::Asset> dragDropAsset = nullptr;
    };
}
