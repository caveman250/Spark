#pragma once

#include <engine/memory/Arena.h>
#include "spark.h"
#include "Archetype.h"
#include "engine/reflect/TypeResolver.h"
#include "Action.h"
#include "UpdateMode.h"
#include "MaybeLockGuard.h"
#include "Relationship.h"

namespace se::ecs
{
    typedef uint64_t Id;
    template <typename... Cs>
    class EngineSystem;
    template <typename... Cs>
    class AppSystem;
}

template <class C>
concept EngineSystemConcept = requires(C c) {
    []<typename... Cs>(se::ecs::EngineSystem<Cs...>&){}(c);
};

template <class C>
concept AppSystemConcept = requires(C c) {
    []<typename... Cs>(se::ecs::AppSystem<Cs...>&){}(c);
};

template<class _Alloc>
    struct std::hash<std::vector<se::ecs::Id, _Alloc>>
{
    std::size_t operator()(const std::vector<se::ecs::Id, _Alloc>& vec) const
    {
        std::size_t seed = vec.size();
        for (auto x: vec)
        {
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

namespace se::ecs
{
    class BaseSystem;
    class Relationship;
    constexpr uint32_t s_InvalidEntity = 0;

    struct EntityRecord
    {
        Archetype* archetype;
        size_t entity_idx; // index/row of entity in component data
    };

    typedef size_t ArchetypeComponentKey; // index of the Component for the given archetype
    using ComponentArchetypeRecord = std::unordered_map<Id, ArchetypeComponentKey>; // Component Keys for each archetype that contains the component
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
        friend class BaseSystem;

    public:
        World();

        void Init();
        void Update();
        void Render();
        void Shutdown();

        Id CreateEntity();
        void DestroyEntity(Id entity);

        template<typename T>
        T* AddComponent(Id entity);

        void AddRelationship(Id entity, const Relationship& relationship);
        void RemoveRelationship(Id entity, const Relationship& relationship);

        template<typename T>
        void RemoveComponent(Id entity);

        template <typename T>
        T* AddSingletonComponent();

        template <typename T>
        void RemoveSingletonComponent();

        template <typename T>
        T* GetSingletonComponent();

        template <AppSystemConcept T>
        void CreateAppSystem(const std::vector<Relationship>& relationships);
        void DestroyAppSystem(Id id);

        template <EngineSystemConcept T>
        void CreateEngineSystem(const std::vector<Relationship>& relationships);
        void DestroyEngineSystem(Id id);

        void RebuildSystemUpdateGroups(std::vector<std::vector<Id>>& updateGroups, std::unordered_map<Id, SystemRecord>& systems);

        template <typename T>
        void RegisterComponent();

    private:
        bool IsRunning() { return m_Running; }

        template<typename ...T, typename Func>
        void Each(Func&& func, const std::vector<Relationship>& relationships, bool force);

        void RegisterRelationship(Id id);

        void RunOnAllAppSystems(const std::function<void(Id)>& func);
        void RunOnAllEngineSystems(const std::function<void(Id)>& func);

        template<typename T>
        T* GetComponent(Id entity);

        struct PendingComponent
        {
            Id entity;
            Id comp;
            void* tempData;
        };

        void AddComponentInternal(const PendingComponent& pendingComp);
        void RemoveComponentInternal(Id entity, Id comp);

        template<typename T>
        bool HasComponent(Id entity);

        Id NewSystem();
        Id RecycleSystem();
        void CreateEngineSystemInternal(Id system, const std::vector<Relationship>& relationships);
        void DestroyEngineSystemInternal(Id system);
        void CreateAppSystemInternal(Id system, const std::vector<Relationship>& relationships);
        void DestroyAppSystemInternal(Id system);

        bool HasComponent(Id entity, Id component);
        void* GetComponent(Id entity, Id component);

        uint32_t NewEntity();
        uint32_t RecycleEntity();
        void DestroyEntityInternal(Id entity);
        size_t MoveEntity(Id entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type);
        bool HasArchetype(const Type& type);
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype, Id component, bool add);

        void ProcessPendingComponents();
        void ProcessPendingSystems();
        void ProcessPendingAppSystems();
        void ProcessPendingEngineSystems();
        void ProcessPendingEntityDeletions();

        bool m_Running = false;

        std::unordered_map<Id, Archetype> m_Archetypes = {};
        std::unordered_map<Type, Id> m_ArchetypeTypeLookup = {};
        std::unordered_map<Id, EntityRecord> m_EntityRecords = {};
        std::unordered_map<Id, ComponentRecord> m_ComponentRecords = {};
        std::unordered_map<Id, void*> m_SingletonComponents = {};
        std::unordered_map<Id, SystemRecord> m_AppSystems = {};
        std::unordered_map<Id, SystemRecord> m_EngineSystems = {};

        uint32_t m_EntityCounter = 1;
        std::vector<uint32_t> m_FreeEntities = {};
        Id m_SystemCounter;
        std::vector<Id> m_FreeSystems = {};
        Id m_ArchetypeCounter = 0;

        std::vector<std::vector<Id>> m_AppSystemUpdateGroups = {};
        std::vector<std::vector<Id>> m_EngineSystemUpdateGroups = {};
        UpdateMode::Type m_UpdateMode = UpdateMode::SingleThreaded;
        std::mutex m_EntityMutex = {};
        std::mutex m_ComponentMutex = {};
        std::mutex m_SystemMutex = {};

        std::vector<Id> m_PendingEntityDeletions;
        std::vector<PendingComponent> m_PendingComponentCreations;
        std::vector<std::pair<Id, Id>> m_PendingComponentDeletions;
        std::vector<std::pair<Id, std::vector<Relationship>>> m_PendingAppSystemCreations;
        std::vector<Id> m_PendingAppSystemDeletions;
        std::vector<std::pair<Id, std::vector<Relationship>>> m_PendingEngineSystemCreations;
        std::vector<Id> m_PendingEngineSystemDeletions;
        memory::Arena m_TempStore; // cleared after all pending creations/deletions
    };

    template<typename T>
    bool World::HasComponent(Id entity)
    {
        RegisterComponent<T>();
        return HasComponent(entity, T::GetComponentId());
    }

    template<typename T>
    T* World::GetComponent(Id entity)
    {
        RegisterComponent<T>();
        return static_cast<T*>(GetComponent(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::RegisterComponent()
    {
        if (T::s_ComponentId == 0)
        {
            if (!m_FreeEntities.empty())
            {
                T::s_ComponentId = bits::PackUtil::Pack64(RecycleEntity(), 0);
            }
            else
            {
                T::s_ComponentId = bits::PackUtil::Pack64(NewEntity(), 0);
            }
        }

        if (!T::IsSingletonComponent() && !m_ComponentRecords.contains(T::GetComponentId()))
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
    void CollectComponentId(std::vector<Id>& compIds)
    {
        if (!T::IsSingletonComponent())
        {
            compIds.push_back(T::GetComponentId());
        }
    }

    void CollectRelationshipIds(std::vector<Id>& compIds, const std::vector<Relationship>& relationships);

    template<typename ...T, typename Func>
    void World::Each(Func&& func, const std::vector<Relationship>& relationships, bool force)
    {
        std::vector<Id> compIds = {};
        compIds.reserve(sizeof...(T) + relationships.size());
        (CollectComponentId<T>(compIds), ...);
        std::unordered_map<Id, std::unordered_map<Id, size_t>> argIndexs;
        CollectRelationshipIds(compIds, relationships);

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

        bool hasStaticComps = compIds.size() - relationships.size() != sizeof...(T);
        if (archetypes.empty() && (force || hasStaticComps))
        {
            Action<T...>::DoAction({}, m_SingletonComponents, nullptr, func);
        }
    }

    template<typename T>
    T* World::AddComponent(Id entity)
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
    void World::RemoveComponent(Id entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterComponent<T>();
        if (!SPARK_VERIFY(HasComponent<T>(entity)))
        {
            return;
        }
        m_PendingComponentDeletions.push_back(std::make_pair(entity, T::GetComponentId()));
    }

    template<EngineSystemConcept T>
    void World::CreateEngineSystem(const std::vector<Relationship>& relationships)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        Id system;
        if (!m_FreeSystems.empty())
        {
            system = RecycleSystem();
        }
        else
        {
            system = NewSystem();
        }
        m_PendingEngineSystemCreations.push_back({system, relationships});
        if (SPARK_VERIFY(!m_EngineSystems.contains(system)))
        {
            m_EngineSystems.insert(std::make_pair(system, SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }
    }

    template <AppSystemConcept T>
    void World::CreateAppSystem(const std::vector<Relationship>& relationships)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        Id system;
        if (!m_FreeSystems.empty())
        {
            system = RecycleSystem();
        }
        else
        {
            system = NewSystem();
        }
        m_PendingAppSystemCreations.push_back({system, relationships});
        if (SPARK_VERIFY(!m_AppSystems.contains(system)))
        {
            m_AppSystems.insert(std::make_pair(system, SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }
    }

    template<typename T>
    T* World::AddSingletonComponent()
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);

        if (!SPARK_VERIFY(!m_SingletonComponents.contains(T::GetComponentId())))
        {
            return nullptr;
        }

        RegisterComponent<T>();
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
