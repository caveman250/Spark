#include "spark.h"
#include "ParentComponent.h"

namespace se::ecs::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(ParentComponent)
        DEFINE_MEMBER(childCount)
    DEFINE_SPARK_COMPONENT_END()
}