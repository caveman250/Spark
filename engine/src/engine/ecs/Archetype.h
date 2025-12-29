#pragma once

#include <Components.generated.h>

#include "spark.h"
#include "ComponentList.h"
#include "ecs_fwd.h"

namespace se::ecs
{
    typedef std::bitset<NumComponents> Type;
    typedef std::vector<Id> TypeVector;

    struct Archetype;
    struct ArchetypeEdge
    {
        Archetype* add = nullptr;
        Archetype* remove = nullptr;
    };

    struct Archetype
    {
        Id id;
        Type type;
        TypeVector typeVector;
        std::vector<Id> entities;
        std::vector<ComponentList> components;
        std::unordered_map<Id, ArchetypeEdge> edges;
    };
}
