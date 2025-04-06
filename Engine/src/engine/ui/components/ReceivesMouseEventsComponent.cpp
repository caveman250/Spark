#include "ReceivesMouseEventsComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(ReceivesMouseEventsComponent)
        DEFINE_SERIALIZED_MEMBER(hovered)
        DEFINE_SERIALIZED_MEMBER(lastHovered)
        DEFINE_SERIALIZED_MEMBER(mouseEvents)
        DEFINE_SERIALIZED_MEMBER(buttonMask)
        DEFINE_SERIALIZED_MEMBER(stateMask)
    DEFINE_SPARK_COMPONENT_END(ReceivesMouseEventsComponent)
}