#pragma once

#include "spark.h"

namespace se::reflect
{
    struct System : Class
    {
        std::function<ecs::Id()> GetStaticId = {};
    };
}