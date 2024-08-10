#include "PointLight.h"

namespace se::render
{
    DEFINE_SPARK_POD_CLASS_BEGIN(PointLight)
        DEFINE_MEMBER(pos)
        DEFINE_MEMBER(color)
    DEFINE_SPARK_CLASS_END()
}