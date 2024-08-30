#include "ReceivesKeyboardEventsComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(ReceivesKeyboardEventsComponent)
        DEFINE_MEMBER(keyEvents)
        DEFINE_MEMBER(keyMask)
        DEFINE_MEMBER(stateMask)
    DEFINE_SPARK_COMPONENT_END()
}