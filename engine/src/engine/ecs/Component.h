#pragma once 

#include "spark.h"
#include "engine/reflect/Object.h"

namespace se::ecs 
{
    struct Component : reflect::ObjectBase
    {
        SPARK_CLASS(Abstract)

        virtual Id GetStaticComponentId() const = 0;
    };
}
