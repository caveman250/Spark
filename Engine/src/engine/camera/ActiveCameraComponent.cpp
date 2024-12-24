#include "spark.h"
#include "ActiveCameraComponent.h"

namespace se::camera
{
    DEFINE_SPARK_COMPONENT_BEGIN(ActiveCameraComponent)
        DEFINE_SERIALIZED_MEMBER(pos)
        DEFINE_SERIALIZED_MEMBER(rot)
        DEFINE_SERIALIZED_MEMBER(proj)
        DEFINE_SERIALIZED_MEMBER(view)
    DEFINE_SPARK_COMPONENT_END()
}