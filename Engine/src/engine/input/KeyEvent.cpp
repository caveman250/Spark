#include "KeyEvent.h"

namespace se::input
{
    DEFINE_SPARK_CLASS_BEGIN(KeyEvent)
        DEFINE_MEMBER(key)
        DEFINE_MEMBER(state)
    DEFINE_SPARK_CLASS_END()
}