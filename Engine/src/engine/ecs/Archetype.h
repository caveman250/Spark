#pragma once

#include "ComponentList.h"

namespace se::ecs
{
    typedef uint64_t Id;
    typedef std::vector<Id> Type;

    struct Archetype;
    struct ArchetypeEdge
    {
        Archetype* add;
        Archetype* remove;
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