#include "spark.h"
#include "RectTransformComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(RectTransformComponent)
        DEFINE_MEMBER(anchors)
        DEFINE_MEMBER(minX)
        DEFINE_MEMBER(minY)
        DEFINE_MEMBER(maxX)
        DEFINE_MEMBER(maxY)
        DEFINE_MEMBER(rect)
    DEFINE_SPARK_COMPONENT_END()
}