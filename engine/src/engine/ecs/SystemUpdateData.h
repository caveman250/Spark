#pragma once

#include "spark.h"

namespace se::ecs
{
    struct SystemUpdateEntityComponentData
    {
        void* componentArray = nullptr;
        ComponentMutability mutability = {};
    };

    struct SystemUpdateSingletonComponentData
    {
        void* component = nullptr;
        ComponentMutability mutability = {};
    };

    struct SystemUpdateVariantComponentData
    {
        void* variant = nullptr;
        Id variant_type = {};
        ComponentMutability mutability = {};
    };

    class SystemUpdateData
    {
    public:
        void SetEntities(const std::vector<Id>& entities)
        {
            m_Entities = &entities;
        }

        const Id& GetEntity() const
        {
            return m_Entities->at(0);
        }

        const std::vector<Id>& GetEntities() const
        {
            return *m_Entities;
        }

        template <typename T>
        T* GetComponentArray() const
        {
#if !SPARK_DIST
            if (!SPARK_VERIFY(m_ComponentArrays.contains(T::GetComponentId()),
                              "SystemUpdateData::GetComponentArray - Component of type {} was not registered for by calling system.", T::GetComponentId().id))
            {
                return nullptr;
            }
#endif
            const SystemUpdateEntityComponentData& entityComponentData = m_ComponentArrays.at(T::GetComponentId());
            if (!SPARK_VERIFY((entityComponentData.mutability == ComponentMutability::Immutable) == std::is_const_v<T>,
                    "SystemUpdateData::GetComponentArray - Component of type {} mutability mismatch", T::GetComponentId().id))
            {
                return nullptr;
            }
            return static_cast<T*>(m_ComponentArrays.at(T::GetComponentId()).componentArray);
        }

        void ClearEntityData()
        {
            m_ComponentArrays.clear();
            m_VariantComponentData = {};
            m_Entities = nullptr;
        }

        template <typename T>
        T* GetSingletonComponent() const
        {
#if !SPARK_DIST
            if (!SPARK_VERIFY(m_SingletonComponents.contains(T::GetComponentId()),
                              "SystemUpdateData::GetComponentArray - Component of type {} was not registered for by calling system.", T::GetComponentId().id))
            {
                return nullptr;
            }
#endif

            const SystemUpdateSingletonComponentData& singletonComponentData = m_SingletonComponents.at(T::GetComponentId());
            if (!SPARK_VERIFY((singletonComponentData.mutability == ComponentMutability::Immutable) == std::is_const_v<T>,
                    "SystemUpdateData::GetComponentArray - Component of type {} mutability mismatch", T::GetComponentId().id))
            {
                return nullptr;
            }
            return static_cast<T*>(singletonComponentData.component);
        }

        template <typename T, typename... Ts>
        void EmplaceVariantArray(const Id& id, void* val, std::variant<Ts...>& variant, bool& didFindType) const
        {
            if (id == T::GetComponentId())
            {
                variant.template emplace<T*>(static_cast<T*>(val));
                didFindType = true;
            }
        }

        template <typename... Ts>
        std::variant<Ts*...> GetVariantComponentArray() const
        {
#if !SPARK_DIST
            bool containsNullType = false;
            (VariantContainsNullType<Ts>(containsNullType), ...);
            if (!SPARK_VERIFY(containsNullType || m_VariantComponentData.variant_type.id != s_InvalidEntity,
                         "SystemUpdateData::GetVariantComponentArray - Variant Component not set."))
            {
                return std::variant<Ts*...>();
            }
#endif

            bool constVariant = true;
            (AreTypesConst<Ts>(constVariant), ...);
            if (!SPARK_VERIFY((m_VariantComponentData.mutability == ComponentMutability::Immutable) == constVariant,
                         "SystemUpdateData::GetVariantComponentArray - Variant Component mutability mismatch"))
            {
                return std::variant<Ts*...>();
            }

            bool didFindType = false;
            std::variant<Ts*...> ret = {};
            (EmplaceVariantArray<Ts>(m_VariantComponentData.variant_type, m_VariantComponentData.variant, ret, didFindType), ...);
            SPARK_ASSERT(didFindType, "SystemUpdateData::GetVariantComponentArray - type mismatch");

            return ret;
        }

        void AddComponentArray(const Id& component, void* compArray, ComponentMutability mutability)
        {
            m_ComponentArrays.insert(
                    std::make_pair(component, SystemUpdateEntityComponentData(compArray, mutability)));
        }

        void AddSingletonComponent(const Id& id, void* compArray, ComponentMutability mutability)
        {
            m_SingletonComponents.insert(
                    std::make_pair(id, SystemUpdateSingletonComponentData(compArray, mutability)));
        }

        void AddVariantComponentArray(const Id& id, void* compArray, ComponentMutability mutability)
        {
            m_VariantComponentData = SystemUpdateVariantComponentData(compArray, id, mutability);
        }

    private:
        template <typename T>
        void AreTypesConst(bool& outConst) const
        {
            outConst &= std::is_const_v<T>;
        }

        template <typename T>
        void VariantContainsNullType(bool& containsNullType) const
        {
            using ComponentType = std::decay_t<T>;
            containsNullType |= std::is_same_v<ComponentType, NullComponentType>;
        }

        std::unordered_map<Id, SystemUpdateEntityComponentData> m_ComponentArrays = {};
        std::unordered_map<Id, SystemUpdateSingletonComponentData> m_SingletonComponents = {};
        SystemUpdateVariantComponentData m_VariantComponentData = {};
        const std::vector<Id>* m_Entities = nullptr;
    };
}