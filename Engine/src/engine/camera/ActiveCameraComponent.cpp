#include "spark.h"
#include "ActiveCameraComponent.h"

namespace se::camera
{
    DEFINE_SPARK_COMPONENT_BEGIN(ActiveCameraComponent)
        DEFINE_MEMBER(pos)
        DEFINE_MEMBER(horizontalAngle)
        DEFINE_MEMBER(verticalAngle)
        DEFINE_MEMBER(proj)
        DEFINE_MEMBER(view)
    DEFINE_SPARK_COMPONENT_END()
}