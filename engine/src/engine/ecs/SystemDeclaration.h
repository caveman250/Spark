#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    class Relationship;

    SPARK_ENUM_BEGIN(ComponentMutability, int)
        Immutable,
        Mutable,
    SPARK_ENUM_END()

    struct ComponentUsage
    {
        template<typename T>
        ComponentUsage(ComponentMutability _mutability)
            : id(T::GetComponentId())
            , mutability(_mutability)
        {
        }

        ComponentUsage(const ecs::Id& component,
                       ComponentMutability _mutability)
            : id(component)
            , mutability(_mutability)
        {
        }

        bool operator==(const ComponentUsage& rhs) const
        {
            return id == rhs.id && mutability == rhs.mutability;
        }

        Id id = 0;
        ComponentMutability mutability;
    };

    struct VariantComponentUsage
    {
        size_t type_hash = 0;
        std::set<ecs::Id> components;
        ComponentMutability mutability;
    };

    struct SystemDeclaration
    {
        SystemDeclaration() = default;

        SystemDeclaration(const String& _name)
            : name(_name)
        {
        }

        template<typename T>
        SystemDeclaration& WithComponent()
        {
            static_assert(!T::IsSingletonComponent());
            componentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                    std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        SystemDeclaration& WithComponent(const ComponentUsage& usage)
        {
            componentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        SystemDeclaration& WithSingletonComponent()
        {
            static_assert(T::IsSingletonComponent());
            singletonComponentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                             std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        SystemDeclaration& WithSingletonComponent(const ComponentUsage& usage)
        {
            singletonComponentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        void CollectVariantComponentIds(std::set<ecs::Id> components)
        {
            components.insert(T::GetComponentId());
        }

        template<typename... Ts>
        SystemDeclaration& WithVariantComponent(ComponentMutability mutability)
        {
            if (!SPARK_VERIFY(variantComponentUsage.type_hash == 0), "Only one variant component supported.")
            {
                return *this;
            }
            std::set<ecs::Id> components;
            (CollectVariantComponentIds<Ts>(components), ...);
            variantComponentUsage = VariantComponentUsage(typeid(std::variant<Ts*...>).hash_code(), components, mutability);
            return *this;
        }

        template<typename T>
        SystemDeclaration& WithChildQuery()
        {
            childQuerys.push_back(ComponentUsage(T::GetComponentId(), std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }


        SystemDeclaration& WithChildQuery(const Id& component,
                                          ComponentMutability mutability)
        {
            childQuerys.push_back(ComponentUsage(component, mutability));
            return *this;
        }

        template<typename... Ts>
        SystemDeclaration& WithChildQuerys(ComponentMutability mutability)
        {
            (childQuerys.push_back(ComponentUsage(Ts::GetComponentId(), mutability)), ...);
            return *this;
        }

        SystemDeclaration& WithDependency(const Id& system_dependency)
        {
            dependencies.insert(system_dependency);
            return *this;
        }

        String name = { };
        std::vector<ComponentUsage> componentUsage = { };
        std::vector<ComponentUsage> singletonComponentUsage = { };
        VariantComponentUsage variantComponentUsage = { };
        std::vector<ComponentUsage> childQuerys = { };
        std::unordered_set<Id> dependencies = { };
    };

    struct ChildQueryDeclaration
    {
        ChildQueryDeclaration() = default;

        template<typename T>
        ChildQueryDeclaration& WithComponent()
        {
            componentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                    std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        ChildQueryDeclaration& WithComponent(const ComponentUsage& usage)
        {
            componentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        ChildQueryDeclaration& WithSingletonComponent()
        {
            singletonComponentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                             std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        ChildQueryDeclaration& WithSingletonComponent(const ComponentUsage& usage)
        {
            singletonComponentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        void CollectVariantComponentIds(std::set<ecs::Id>& components)
        {
            components.insert(T::GetComponentId());
        }

        template<typename... Ts>
        ChildQueryDeclaration& WithVariantComponent(ComponentMutability mutability)
        {
            if (!SPARK_VERIFY(variantComponentUsage.type_hash == 0, "Only one variant component supported."))
            {
                return *this;
            }
            std::set<ecs::Id> components;
            (CollectVariantComponentIds<Ts>(components), ...);
            variantComponentUsage = VariantComponentUsage(typeid(std::variant<Ts*...>).hash_code(), components, mutability);
            return *this;
        }

        std::vector<ComponentUsage> componentUsage = { };
        std::vector<ComponentUsage> singletonComponentUsage = { };
        VariantComponentUsage variantComponentUsage = { };
    };
}
