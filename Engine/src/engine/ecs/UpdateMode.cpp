#include "UpdateMode.h"

DEFINE_SPARK_ENUM_BEGIN(se::ecs::UpdateMode)
    DEFINE_ENUM_VALUE(UpdateMode, SingleThreaded)
    DEFINE_ENUM_VALUE(UpdateMode, MultiThreaded)
DEFINE_SPARK_ENUM_END()