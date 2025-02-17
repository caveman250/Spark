#include "spark.h"
#include "TransformComponent.h"

namespace se::ecs::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(TransformComponent)
        DEFINE_SERIALIZED_MEMBER(pos)
        DEFINE_SERIALIZED_MEMBER(rot)
        DEFINE_SERIALIZED_MEMBER(scale)
    DEFINE_SPARK_COMPONENT_END(TransformComponent)
}