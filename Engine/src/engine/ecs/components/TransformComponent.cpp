#include "spark.h"
#include "TransformComponent.h"

namespace se::ecs::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(TransformComponent)
        DEFINE_MEMBER(pos)
        DEFINE_MEMBER(rot)
        DEFINE_MEMBER(scale)
    DEFINE_SPARK_COMPONENT_END()
}