#include "KeyInputComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(KeyInputComponent)
        DEFINE_SERIALIZED_MEMBER(keyEvents)
        DEFINE_SERIALIZED_MEMBER(keyMask)
        DEFINE_SERIALIZED_MEMBER(stateMask)
    DEFINE_SPARK_COMPONENT_END(KeyInputComponent)
}