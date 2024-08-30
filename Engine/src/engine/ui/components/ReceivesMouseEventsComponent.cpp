#include "ReceivesMouseEventsComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(ReceivesMouseEventsComponent)
        DEFINE_MEMBER(mouseEvents)
        DEFINE_MEMBER(buttonMask)
        DEFINE_MEMBER(stateMask)
    DEFINE_SPARK_COMPONENT_END()
}