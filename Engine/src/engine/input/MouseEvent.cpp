#include "MouseEvent.h"

namespace se::input
{
    DEFINE_SPARK_CLASS_BEGIN(MouseEvent)
        DEFINE_SERIALIZED_MEMBER(button)
        DEFINE_SERIALIZED_MEMBER(state)
    DEFINE_SPARK_CLASS_END()
}