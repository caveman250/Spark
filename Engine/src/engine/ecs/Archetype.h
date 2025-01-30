#pragma once

#include "ComponentList.h"
#include "ecs_fwd.h"

namespace se::ecs
{
    typedef std::vector<Id> Type;

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
        std::vector<Id> entities;
        std::vector<ComponentList> components;
        std::unordered_map<Id, ArchetypeEdge> edges;
    };
}
