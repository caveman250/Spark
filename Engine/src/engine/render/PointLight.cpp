#include "PointLight.h"

namespace se::render
{
    DEFINE_SPARK_POD_CLASS_BEGIN(PointLight)
        DEFINE_SERIALIZED_MEMBER(pos)
        DEFINE_SERIALIZED_MEMBER(color)
    DEFINE_SPARK_CLASS_END()
}