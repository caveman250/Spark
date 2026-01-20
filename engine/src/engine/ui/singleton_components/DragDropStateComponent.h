#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::asset
{
    class Asset;
}

namespace se::ui::singleton_components
{
    struct DragDropStateComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::shared_ptr<asset::Asset> dragDropAsset = nullptr;
    };
}
