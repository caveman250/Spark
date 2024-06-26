#pragma once

#include <engine/memory/Arena.h>
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

        void Init();
        void Update();
        void Render();
        void Shutdown();

        EntityId CreateEntity();
        void DestroyEntity(EntityId entity);

        template<typename T>
        T* AddComponent(EntityId entity);

        template<typename T>
        void RemoveComponent(EntityId entity);

        template <typename T>
        T* AddSingletonComponent();

        template <typename T>
        void RemoveSingletonComponent();

        template <typename T>
        T* GetSingletonComponent();

        template <typename T>
        void CreateSystem();

        template <typename T>
        void DestroySystem();

        template <typename... T>
        void RegisterSystemUpdateGroup();

        template<typename ...T, typename Func>
        void Each(Func&& func, bool force);

    private:
        bool IsRunning() { return m_Running; }

        template<typename T>
        void RegisterComponent();

        template<typename T>
        void RegisterSystem();

        void RunOnAllSystems(const std::function<void(SystemId)>& func);

        template<typename T>
        T* GetComponent(EntityId entity);

        struct PendingComponent
        {
            EntityId entity;
            ComponentId comp;
            void* tempData;
        };

        void AddComponentInternal(const PendingComponent& pendingComp);
        void RemoveComponentInternal(EntityId entity, ComponentId comp);

        template<typename T>
        bool HasComponent(EntityId entity);

        void CreateSystemInternal(SystemId system);
        void DestroySystemInternal(SystemId system);

        bool HasComponent(EntityId entity, ComponentId component);
        void* GetComponent(EntityId entity, ComponentId component);

        EntityId NewEntity();
        EntityId RecycleEntity();
        void DestroyEntityInternal(EntityId entity);
        size_t MoveEntity(EntityId entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type);
        bool HasArchetype(const Type& type);
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype, ComponentId component, bool add);

        void ProcessPendingComponents();
        void ProcessPendingSystems();
        void ProcessPendingEntityDeletions();

        bool m_Running = false;

        std::unordered_map<ArchetypeId, Archetype> m_Archetypes;
        std::unordered_map<Type, ArchetypeId> m_ArchetypeTypeLookup;
        std::unordered_map<EntityId, EntityRecord> m_EntityRecords;
        std::unordered_map<ComponentId, ComponentRecord> m_ComponentRecords;
        std::unordered_map<ComponentId, void*> m_SingletonComponents;
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
        std::vector<PendingComponent> m_PendingComponentCreations;
        std::vector<std::pair<EntityId, ComponentId>> m_PendingComponentDeletions;
        std::vector<SystemId> m_PendingSystemCreations;
        std::vector<SystemId> m_PendingSystemDeletions;
        memory::Arena m_TempStore; // cleared after all pending creations/deletions
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
        static_assert(!T::IsSingletonComponent());
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

    template <typename T>
    void CollectComponentId(std::vector<ComponentId>& compIds)
    {
        if (!T::IsSingletonComponent())
        {
            compIds.push_back(T::GetComponentId());
        }
    }

    template<typename ...T, typename Func>
    void World::Each(Func&& func, bool force)
    {
        std::vector<ComponentId> compIds = {};
        (CollectComponentId<T>(compIds), ...);
        std::unordered_map<ArchetypeId, std::unordered_map<ComponentId, size_t>> argIndexs;

        std::set<Archetype*> archetypes;
        for (auto compId: compIds)
        {
            if (m_ComponentRecords.contains(compId))
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
                            compKeys.insert(std::make_pair(comp, std::ranges::find(archetype.type, comp) -
                                                                 archetype.type.begin()));
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
        }

        for (auto* archetype: archetypes)
        {
            Action<T...>::DoAction(archetype->entities, m_SingletonComponents, archetype, func);
        }

        bool hasStaticComps = compIds.size() != sizeof...(T);
        if (archetypes.empty() && (force || hasStaticComps))
        {
            Action<T...>::DoAction({}, m_SingletonComponents, nullptr, func);
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
            if (!SPARK_VERIFY(componentCounts[type] == 1, "Cannot run systems in parallel if a component is mutable in one or more systems and accessed by multiple systems"))
            {
                return;
            }
        }
#endif

        m_SystemUpdateGroups.push_back(updateGroup);
    }

    template<typename T>
    T* World::AddComponent(EntityId entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterComponent<T>();

        if (!SPARK_VERIFY(!HasComponent<T>(entity)))
        {
            return nullptr;
        }

        T* tempComp = m_TempStore.Alloc<T>();
        m_PendingComponentCreations.emplace_back(PendingComponent { .entity=entity, .comp=T::GetComponentId(), .tempData=tempComp });
        return tempComp;
    }

    template<typename T>
    void World::RemoveComponent(EntityId entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterComponent<T>();
        if (!SPARK_VERIFY(HasComponent<T>(entity)))
        {
            return;
        }
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

    template<typename T>
    T* World::AddSingletonComponent()
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);

        if (!SPARK_VERIFY(!m_SingletonComponents.contains(T::GetComponentId())))
        {
            return nullptr;
        }

        m_SingletonComponents.insert(std::make_pair(T::GetComponentId(), new T()));
        return static_cast<T*>(m_SingletonComponents.at(T::GetComponentId()));
    }

    template<typename T>
    void World::RemoveSingletonComponent()
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);

        if (!SPARK_VERIFY(m_SingletonComponents.contains(T::GetComponentId())))
        {
            return;
        }

        delete m_SingletonComponents.at(T::GetComponentId());
        m_SingletonComponents.erase(T::GetComponentId());
    }

    template <typename T>
    T* World::GetSingletonComponent()
    {
        if (!SPARK_VERIFY(!IsRunning()))
        {
            return nullptr;
        }

        if (!SPARK_VERIFY(m_SingletonComponents.contains(T::GetComponentId())))
        {
            return nullptr;
        }

        return static_cast<T*>(m_SingletonComponents.at(T::GetComponentId()));
    }
}
