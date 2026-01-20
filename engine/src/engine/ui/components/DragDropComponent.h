#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::ui::components 
{
    struct DragDropComponent : ecs::Component
    {
        SPARK_COMPONENT()
        
        bool pendingDestroy = false;
    };
}