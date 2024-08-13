#pragma once

#include "spark.h"
#include "engine/ecs/RelationshipComponent.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs::components
{
    struct ChildOf : RelationshipComponent
    {
        DECLARE_SPARK_COMPONENT(ChildOf)
    };
}
