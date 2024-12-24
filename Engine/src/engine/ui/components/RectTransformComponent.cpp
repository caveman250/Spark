#include "spark.h"
#include "RectTransformComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(RectTransformComponent)
        DEFINE_SERIALIZED_MEMBER(anchors)
        DEFINE_SERIALIZED_MEMBER(minX)
        DEFINE_SERIALIZED_MEMBER(minY)
        DEFINE_SERIALIZED_MEMBER(maxX)
        DEFINE_SERIALIZED_MEMBER(maxY)
        DEFINE_SERIALIZED_MEMBER(rect)
    DEFINE_SPARK_COMPONENT_END()
}