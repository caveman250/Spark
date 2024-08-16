#include "Rect.h"

namespace se::ui
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Rect)
        DEFINE_MEMBER(topLeft)
        DEFINE_MEMBER(bottomRight)
    DEFINE_SPARK_CLASS_END()
}