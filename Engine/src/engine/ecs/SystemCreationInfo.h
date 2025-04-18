#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    class Relationship;

    DECLARE_SPARK_ENUM_BEGIN(ComponentMutability, int)
        Immutable,
        Mutable,
    DECLARE_SPARK_ENUM_END()

    typedef std::pair<Id, ComponentMutability::Type> ChildQuery;

    struct SystemCreationInfo
    {
        SystemCreationInfo(const String& _name) : name(_name) {}

        SystemCreationInfo& WithRelationship(const Relationship& relationship)
        {
            relationships.push_back(relationship);
            return *this;
        }

        SystemCreationInfo& WithChildQuery(const Id& component, ComponentMutability::Type mutability)
        {
            childQuerys.push_back(std::make_pair(component, mutability));
            return *this;
        }

        SystemCreationInfo& WithDependency(const Id& system_dependency)
        {
            dependencies.insert(system_dependency);
            return *this;
        }

        const String& name;
        std::vector<Relationship> relationships = {};
        std::vector<ChildQuery> childQuerys = {};
        std::unordered_set<Id> dependencies = {};
    };
}
