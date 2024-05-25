#include "UpdateMode.h"

namespace se::ecs
{
    DEFINE_SPARK_ENUM_BEGIN(UpdateMode)
        DEFINE_ENUM_VALUE(UpdateMode, SingleThreaded)
        DEFINE_ENUM_VALUE(UpdateMode, MultiThreaded)
    DEFINE_SPARK_ENUM_END()
}