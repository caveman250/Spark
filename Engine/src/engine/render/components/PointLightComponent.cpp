#include "PointLightComponent.h"

namespace se::render::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(PointLightComponent)
        DEFINE_SERIALIZED_MEMBER(color)
    DEFINE_SPARK_COMPONENT_END(PointLightComponent)
}