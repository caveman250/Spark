#pragma once

#include "Component.h"
#include "ComponentList.h"

namespace se::ecs
{
    typedef uint64_t ArchetypeId;
    typedef std::vector<ComponentId> Type;

    struct Archetype;
    struct ArchetypeEdge
    {
        Archetype* add;
        Archetype* remove;
    };

    struct Archetype
    {
        ArchetypeId id;
        Type type;
        std::vector<ComponentList> components;
        std::unordered_map<ComponentId, ArchetypeEdge> edges;
    };
}