#include "Anchors.h"

namespace se::ui
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Anchors)
        DEFINE_SERIALIZED_MEMBER(left)
        DEFINE_SERIALIZED_MEMBER(right)
        DEFINE_SERIALIZED_MEMBER(top)
        DEFINE_SERIALIZED_MEMBER(bottom)
    DEFINE_SPARK_CLASS_END(Anchors)
}