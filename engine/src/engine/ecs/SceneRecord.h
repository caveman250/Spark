#pragma once

#include "spark.h"

namespace se::ecs
{
    struct SceneRecord
    {
        std::string path = {};
        std::vector<Id> entities = {};
    };
}
