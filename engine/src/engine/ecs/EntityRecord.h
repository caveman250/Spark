#pragma once 

#include "spark.h"

namespace se::ecs
{
    struct EntityRecord
    {
        Archetype* archetype = nullptr;
        size_t entity_idx = {}; // index/row of entity in component data
        Id parent = InvalidEntity;
        std::vector<Id> children = { };
    };
}