#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::ui::components 
{
    struct ContextMenuComponent : ecs::Component
    {
        SPARK_COMPONENT()
    };
}