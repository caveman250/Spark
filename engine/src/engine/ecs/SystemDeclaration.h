#pragma once

#include "spark.h"

namespace se::ecs
{
    class Relationship;

    SPARK_ENUM()
    enum class ComponentMutability
    {
        Immutable,
        Mutable,
    };

    struct ComponentUsage
    {
        template<typename T>
        ComponentUsage(ComponentMutability _mutability)
            : id(T::GetComponentId())
            , mutability(_mutability)
        {
        }

        ComponentUsage(const Id& component,
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
        std::set<Id> components;
        ComponentMutability mutability;
    };

    struct SystemDeclaration
    {
        SystemDeclaration() = default;

        SystemDeclaration(const std::string& _name)
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
        void CollectVariantComponentIds(std::set<Id> components)
        {
            components.insert(T::GetComponentId());
        }

        template<typename... Ts>
        SystemDeclaration& WithVariantComponent(ComponentMutability mutability)
        {
            if (!SPARK_VERIFY(variantComponentUsage.type_hash == 0, "Only one variant component supported."))
            {
                return *this;
            }
            std::set<Id> components;
            (CollectVariantComponentIds<Ts>(components), ...);
            variantComponentUsage = VariantComponentUsage(typeid(std::variant<Ts*...>).hash_code(), components, mutability);
            return *this;
        }

        template<typename T>
        SystemDeclaration& WithHeirachyQuery()
        {
            childQuerys.push_back(ComponentUsage(T::GetComponentId(), std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }


        SystemDeclaration& WithHeirachyQuery(const Id& component,
                                          ComponentMutability mutability)
        {
            childQuerys.push_back(ComponentUsage(component, mutability));
            return *this;
        }

        template<typename... Ts>
        SystemDeclaration& WithHeirachyQuerys(ComponentMutability mutability)
        {
            (childQuerys.push_back(ComponentUsage(Ts::GetComponentId(), mutability)), ...);
            return *this;
        }

        template <typename T>
        SystemDeclaration& WithDependency()
        {
            dependencies.push_back(T::GetSystemId());
            return *this;
        }

        std::string name = { };
        std::vector<ComponentUsage> componentUsage = { };
        std::vector<ComponentUsage> singletonComponentUsage = { };
        VariantComponentUsage variantComponentUsage = { };
        std::vector<ComponentUsage> childQuerys = { };
        std::vector<Id> dependencies = { };
    };

    struct HeirachyQueryDeclaration
    {
        HeirachyQueryDeclaration() = default;

        template<typename T>
        HeirachyQueryDeclaration& WithComponent()
        {
            componentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                    std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        HeirachyQueryDeclaration& WithComponent(const ComponentUsage& usage)
        {
            componentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        HeirachyQueryDeclaration& WithSingletonComponent()
        {
            singletonComponentUsage.push_back(ComponentUsage(T::GetComponentId(),
                                                             std::is_const_v<T> ? ComponentMutability::Immutable : ComponentMutability::Mutable));
            return *this;
        }

        HeirachyQueryDeclaration& WithSingletonComponent(const ComponentUsage& usage)
        {
            singletonComponentUsage.push_back(usage);
            return *this;
        }

        template<typename T>
        void CollectVariantComponentIds(std::set<Id>& components)
        {
            components.insert(T::GetComponentId());
        }

        template<typename... Ts>
        HeirachyQueryDeclaration& WithVariantComponent(ComponentMutability mutability)
        {
            if (!SPARK_VERIFY(variantComponentUsage.type_hash == 0, "Only one variant component supported."))
            {
                return *this;
            }
            std::set<Id> components;
            (CollectVariantComponentIds<Ts>(components), ...);
            variantComponentUsage = VariantComponentUsage(typeid(std::variant<Ts*...>).hash_code(), components, mutability);
            return *this;
        }

        std::vector<ComponentUsage> componentUsage = { };
        std::vector<ComponentUsage> singletonComponentUsage = { };
        VariantComponentUsage variantComponentUsage = { };
    };
}
