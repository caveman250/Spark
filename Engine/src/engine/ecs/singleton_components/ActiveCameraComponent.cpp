#include "spark.h"
#include "ActiveCameraComponent.h"

namespace se::ecs::singleton_components
{
    DEFINE_SPARK_COMPONENT_BEGIN(ActiveCameraComponent)
        DEFINE_MEMBER(proj)
        DEFINE_MEMBER(view)
    DEFINE_SPARK_COMPONENT_END()
}