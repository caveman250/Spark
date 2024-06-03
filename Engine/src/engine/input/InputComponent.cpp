#include "spark.h"
#include "InputComponent.h"

namespace se::input
{
    DEFINE_SPARK_COMPONENT_BEGIN(InputComponent)
        DEFINE_MEMBER(keyEvents)
    DEFINE_SPARK_COMPONENT_END()
}