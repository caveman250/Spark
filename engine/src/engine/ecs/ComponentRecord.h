#pragma once 

#include "spark.h"

namespace se::ecs 
{
    typedef size_t ArchetypeComponentKey; // index of the Component for the given archetype
    using ComponentArchetypeRecord = std::unordered_map<Id, ArchetypeComponentKey>; // Component Keys for each archetype that contains the component
    struct ComponentRecord
    {
        reflect::Class* type = nullptr;
        ComponentArchetypeRecord archetypeRecords = {};
    };
}