#pragma once

#include "spark.h"

namespace se::ecs
{
    typedef uint8_t SceneId;
    struct SceneRecord
    {
        std::vector<Id> entities = {};
    };
}
