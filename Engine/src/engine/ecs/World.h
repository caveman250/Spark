#pragma once

#include "spark.h"
#include "Archetype.h"
#include "engine/reflect/TypeResolver.h"
#include "engine/reflect/Type.h"
#include "ComponentList.h"
#include "Action.h"
#include "UpdateMode.h"
#include "MaybeLockGuard.h"

namespace se::ecs
{
    class BaseSystem;
    typedef uint64_t SystemId;
    typedef uint64_t EntityId;
    constexpr uint64_t s_InvalidEntity = 0xffffffff;

    struct EntityRecord
    {
        Archetype* archetype;
        size_t entity_idx; // index/row of entity in component data
    };

    typedef size_t ArchetypeComponentKey; // index of the Component for the given archetype
    using ComponentArchetypeRecord = std::unordered_map<ArchetypeId, ArchetypeComponentKey>; // Component Keys for each archetype that contains the component
    struct ComponentRecord
    {
        reflect::Class* type;
        ComponentArchetypeRecord archetypeRecords;
    };

    struct SystemRecord
    {
        reflect::Class* type;
        BaseSystem* instance;
    };

    class World
    {
    public:
        World();

        void Update(float dt);

        EntityId CreateEntity();
        void DestroyEntity(EntityId entity);

        template<typename T>
        void AddComponent(EntityId entity);

        template<typename T>
        void RemoveComponent(EntityId entity);

        template <typename T>
        void CreateSystem();

        template <typename T>
        void DestroySystem();

        template <typename... T>
        void RegisterSystemUpdateGroup();

        template<typename ...T, typename Func>
        void Each(float dt, Func&& func);
    private:
        template<typename T>
        void RegisterComponent();

        template<typename T>
        void RegisterSystem();

        template<typename T>
        T* GetComponent(EntityId entity);

        void AddComponentInternal(EntityId entity, ComponentId comp);
        void RemoveComponentInternal(EntityId entity, ComponentId comp);

        template<typename T>
        bool HasComponent(EntityId entity);

        void CreateSystemInternal(SystemId system);
        void DestroySystemInternal(SystemId system);

        bool HasComponent(EntityId entity, ComponentId component);
        void* GetComponent(EntityId entity, ComponentId component);
        void AddComponent(EntityId entity, ComponentId component);
        void RemoveComponent(EntityId entity, ComponentId component);

        EntityId NewEntity();
        EntityId RecycleEntity();
        void DestroyEntityInternal(EntityId entity);
        size_t MoveEntity(Archetype* archetype, size_t entityIdx, Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type);
        bool HasArchetype(const Type& type);
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype, ComponentId component, bool add);

        void ProcessPendingComponents();
        void ProcessPendingSystems();
        void ProcessPendingEntityDeletions();

        std::unordered_map<ArchetypeId, Archetype> m_Archetypes;
        std::unordered_map<Type, ArchetypeId> m_ArchetypeTypeLookup;
        std::unordered_map<EntityId, EntityRecord> m_EntityRecords;
        std::unordered_map<ComponentId, ComponentRecord> m_ComponentRecords;
        std::unordered_map<SystemId, SystemRecord> m_Systems;

        EntityId m_EntityCounter = 0;
        std::vector<EntityId> m_FreeEntities = {};

        ArchetypeId m_ArchetypeCounter = 0;

        std::vector<std::vector<SystemId>> m_SystemUpdateGroups = {};
        UpdateMode::Type m_UpdateMode = UpdateMode::SingleThreaded;
        std::mutex m_EntityMutex = {};
        std::mutex m_ComponentMutex = {};
        std::mutex m_SystemMutex = {};

        std::vector<EntityId> m_PendingEntityDeletions;
        std::vector<std::pair<EntityId, ComponentId>> m_PendingComponentCreations;
        std::vector<std::pair<EntityId, ComponentId>> m_PendingComponentDeletions;
        std::vector<SystemId> m_PendingSystemCreations;
        std::vector<SystemId> m_PendingSystemDeletions;
    };

    template<typename T>
    bool World::HasComponent(EntityId entity)
    {
        RegisterComponent<T>();
        return HasComponent(entity, T::GetComponentId());
    }

    template<typename T>
    T* World::GetComponent(EntityId entity)
    {
        RegisterComponent<T>();
        return static_cast<T*>(GetComponent(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::RegisterComponent()
    {
        if (!m_ComponentRecords.contains(T::GetComponentId()))
        {
            m_ComponentRecords.insert(std::make_pair(T::GetComponentId(),
                                                     ComponentRecord
                                                             {
                                                                     .type = reflect::ClassResolver<T>::get(),
                                                                     .archetypeRecords = {}
                                                             }));
        }
    }

    template<typename ...T, typename Func>
    void World::Each(float dt, Func&& func)
    {
        std::vector<ComponentId> compIds = {(T::GetComponentId(), ...)};
        std::unordered_map<ArchetypeId, std::unordered_map<ComponentId, size_t>> argIndexs;

        std::set<Archetype*> archetypes;
        for (auto compId: compIds)
        {
            auto& compRecord = m_ComponentRecords[compId];
            for (const auto& [archetypeId, _]: compRecord.archetypeRecords)
            {
                auto& archetype = m_Archetypes.at(archetypeId);

                if (!argIndexs.contains(archetypeId))
                {
                    auto& compKeys = argIndexs[archetypeId];
                    for (auto comp: compIds)
                    {
                        compKeys.insert(
                                std::make_pair(comp, std::ranges::find(archetype.type, comp) - archetype.type.begin()));
                    }
                }

                bool containsAll = true;
                for (auto comp: compIds)
                {
                    containsAll &= std::ranges::contains(archetype.type, comp);
                }

                if (!containsAll)
                {
                    continue;
                }

                archetypes.insert(&archetype);
            }
        }

        for (auto* archetype: archetypes)
        {
            Action<T...>::DoAction(dt, archetype, func);
        }
    }

    template<typename T>
    void World::RegisterSystem()
    {
        if (!m_Systems.contains(T::GetSystemId()))
        {
            m_Systems.insert(std::make_pair(T::GetSystemId(), SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }

    }

    template<typename... T>
    void World::RegisterSystemUpdateGroup()
    {
        std::vector<SystemId> updateGroup;
        ((updateGroup.emplace_back(T::GetSystemId())), ...);

#if SPARK_DEBUG
        std::unordered_map<SystemId, std::vector<std::pair<ComponentId, bool>>> usedComponents;
        (usedComponents.insert(std::make_pair(T::GetSystemId(), T::GetUsedComponents())), ...);

        std::set<ComponentId> mutableComponents;
        std::unordered_map<ComponentId, int> componentCounts;
        for (auto systemId : updateGroup)
        {
            for (const auto& [comp, isConst] : usedComponents[systemId])
            {
                if (!isConst)
                {
                    mutableComponents.insert(comp);
                }

                componentCounts[comp]++;
            }
        }

        for (auto type : mutableComponents)
        {
            SPARK_ASSERT(componentCounts[type] == 1, "Cannot run systems in parallel if a component is mutable in one or more systems and accessed by multiple systems");
        }


#endif

        m_SystemUpdateGroups.push_back(updateGroup);
    }

    template<typename T>
    void World::AddComponent(EntityId entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterComponent<T>();
        m_PendingComponentCreations.push_back(std::make_pair(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::RemoveComponent(EntityId entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterComponent<T>();
        m_PendingComponentDeletions.push_back(std::make_pair(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::CreateSystem()
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        RegisterSystem<T>();
        m_PendingSystemCreations.push_back(T::GetSystemId());
    }

    template<typename T>
    void World::DestroySystem()
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        RegisterSystem<T>();
        m_PendingSystemDeletions.push_back(T::GetSystemId());
    }
}
