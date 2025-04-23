#include "MouseInputComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(MouseInputComponent)
        DEFINE_SERIALIZED_MEMBER(hovered)
        DEFINE_SERIALIZED_MEMBER(lastHovered)
        DEFINE_SERIALIZED_MEMBER(mouseEvents)
        DEFINE_SERIALIZED_MEMBER(buttonMask)
        DEFINE_SERIALIZED_MEMBER(stateMask)
    DEFINE_SPARK_COMPONENT_END(MouseInputComponent)
}