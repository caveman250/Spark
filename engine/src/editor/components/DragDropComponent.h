#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::editor::components
{
    struct DragDropComponent : ecs::Component
    {
        SPARK_COMPONENT(EditorOnly)
        
        bool pendingDestroy = false;
    };
}