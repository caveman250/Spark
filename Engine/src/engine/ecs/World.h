#pragma once

#include <engine/memory/Arena.h>
#include "spark.h"
#include "Archetype.h"
#include "engine/reflect/TypeResolver.h"
#include "Action.h"
#include "UpdateMode.h"
#include "MaybeLockGuard.h"
#include "Observer.h"
#include "Relationship.h"

namespace se::ecs::components
{
    struct ChildOf;
}

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

template<class Alloc>
struct std::hash<std::vector<se::ecs::Id, Alloc>>
{
    std::size_t operator()(const std::vector<se::ecs::Id, Alloc>& vec) const
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
    template<typename... Cs>
    class SignalActionBuilder;
    class BaseSignal;
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

    DECLARE_SPARK_ENUM_BEGIN(ComponentMutability, int)
        Immutable,
        Mutable,
    DECLARE_SPARK_ENUM_END()

    typedef std::vector<std::pair<Id, ComponentMutability::Type>> ChildQuery;
    struct PendingSystemInfo
    {
        std::vector<Relationship> relationships;
        ChildQuery childQuery;
        std::unordered_set<Id> dependsOn;
    };

    struct SystemRecord
    {
        reflect::Class* type;
        BaseSystem* instance;
    };

    class World
    {
        friend class BaseSystem;
        template <typename... Cs>
        friend class Signal;
        template<typename... Cs>
        friend class SignalActionBuilder;

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
        void AddChild(Id entity, Id childEntity);
        void RemoveChild(Id entity, Id childEntity);

        template<typename T>
        void RemoveComponent(Id entity);

        template <typename T>
        T* AddSingletonComponent();

        template <typename T>
        void RemoveSingletonComponent();

        template <typename T>
        T* GetSingletonComponent();

        template <AppSystemConcept T>
        Id CreateAppSystem(const std::vector<Relationship>& relationships, const ChildQuery& additionalChildQuery, const std::unordered_set<Id>& dependsOn);
        void DestroyAppSystem(Id id);

        template <EngineSystemConcept T>
        Id CreateEngineSystem(const std::vector<Relationship>& relationships, const ChildQuery& additionalChildQuery, const std::unordered_set<Id>& dependsOn);
        void DestroyEngineSystem(Id id);

        template <typename T>
        void RegisterComponent();
        template <typename T>
        std::pair<Id, ComponentMutability::Type> CreateComponentUsagePair(ComponentMutability::Type type);

        template <typename T, typename Y>
        Relationship CreateComponentRelationship();

        template<typename T>
        bool HasComponent(Id entity);

        template<typename T, typename Y>
        bool HasRelationship(Id entity);

        template<typename T>
        bool HasRelationshipWildcard(Id entity);

        bool IsChildOf(Id entity, Id parent);

        template<typename T, typename Y>
        Id CreateObserver();

        void DestroyObserver(Id id);

    private:
        bool IsRunning() const { return m_Running; }

        template<typename ...T, typename Func>
        void Each(Func&& func, const std::vector<Relationship>& relationships, bool force);

        template<typename ...T, typename Func>
        void ChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships);

        template<typename ...T, typename Func>
        void RecursiveChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships);

        bool ValidateChildQuery(BaseSystem* system, const std::vector<std::pair<Id, ComponentMutability::Type>>& requestedComponents);
        Relationship CreateChildRelationship(Id entity);
        bool HasRelationshipWildcardInternal(Id entity, uint32_t lhs);

        void RegisterRelationship(Id id);

        void RunOnAllSystems(const std::function<void(Id)>& func, const std::vector<std::vector<Id>>& systems, bool parallel, bool processPending);
        void RunOnAllAppSystems(const std::function<void(Id)>& func, bool parallel, bool processPending);
        void RunOnAllEngineSystems(const std::function<void(Id)>& func, bool parallel, bool processPending);

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

        Id NewSystem();
        Id RecycleSystem();

        static void CreateSystemInternal(std::unordered_map<Id, SystemRecord>& systemMap, std::unordered_map<Id, ChildQuery>& allowedChildQueries, Id system, const PendingSystemInfo& pendingSystem);
        static void DestroySystemInternal(std::unordered_map<Id, SystemRecord>& systemMap, std::unordered_map<Id, ChildQuery>& allowedChildQueries, std::vector<Id>& freeSystems, Id system);

        Id NewObserver();
        Id RecycleObserver();

        bool HasComponent(Id entity, Id component);
        void* GetComponent(Id entity, Id component);

        uint32_t NewEntity();
        uint32_t RecycleEntity();
        void DestroyEntityInternal(Id entity);
        size_t MoveEntity(Id entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type);
        bool HasArchetype(const Type& type) const;
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype, Id component, bool add);

        void ProcessPendingComponents();
        void ProcessPendingSystems();
        static void ProcessPendingSystems(std::vector<std::pair<Id, PendingSystemInfo>>& pendingCreations,
                                    std::vector<Id>& pendingDeletions,
                                    std::unordered_map<Id, SystemRecord>& systemRecords,
                                    std::vector<std::vector<Id>>& systsemUpdateGroups,
                                    std::vector<Id>& freeSystems,
                                    std::unordered_map<Id, ChildQuery>& allowedChildQueries);
        static void RebuildSystemUpdateGroups(std::vector<std::vector<Id>>& updateGroups, std::unordered_map<Id, SystemRecord>& systems);
        void ProcessPendingAppSystems();
        void ProcessPendingEngineSystems();
        void ProcessPendingEntityDeletions();

        void AddPendingSignal(BaseSignal* signal);
        void OnSignalDestroyed(BaseSignal* signal);

        bool m_Running = false;

        std::unordered_map<Id, Archetype> m_Archetypes = {};
        std::unordered_map<Type, Id> m_ArchetypeTypeLookup = {};
        std::unordered_map<Id, EntityRecord> m_EntityRecords = {};
        std::unordered_map<Id, ComponentRecord> m_ComponentRecords = {};
        std::unordered_map<Id, void*> m_SingletonComponents = {};
        std::unordered_map<Id, SystemRecord> m_AppSystems = {};
        std::unordered_map<Id, SystemRecord> m_EngineSystems = {};
        std::unordered_map<Id, ChildQuery> m_AllowedChildQueries = {};
        std::unordered_map<Id, std::unordered_map<Id, std::shared_ptr<BaseObserver>>> m_Observers;

        uint32_t m_EntityCounter = 1;
        std::vector<uint32_t> m_FreeEntities = {};
        Id m_SystemCounter = 0;
        std::vector<Id> m_FreeSystems = {};
        Id m_ArchetypeCounter = 0;
        Id m_ObserverCounter = 0;
        std::vector<Id> m_FreeObservers = {};

        std::vector<std::vector<Id>> m_AppSystemUpdateGroups = {};
        std::vector<std::vector<Id>> m_EngineSystemUpdateGroups = {};
        UpdateMode::Type m_UpdateMode = UpdateMode::SingleThreaded;
        std::mutex m_EntityMutex = {};
        std::mutex m_ComponentMutex = {};
        std::mutex m_SystemMutex = {};
        std::mutex m_ObserverMutex = {};

        std::vector<Id> m_PendingEntityDeletions;
        std::vector<PendingComponent> m_PendingComponentCreations;
        std::vector<std::pair<Id, Id>> m_PendingComponentDeletions;

        std::vector<std::pair<Id, PendingSystemInfo>> m_PendingAppSystemCreations;
        std::vector<Id> m_PendingAppSystemDeletions;
        std::vector<std::pair<Id, PendingSystemInfo>> m_PendingEngineSystemCreations;
        std::vector<Id> m_PendingEngineSystemDeletions;
        memory::Arena m_TempStore; // cleared after all pending creations/deletions

        std::vector<BaseSignal*> m_PendingSignals;
    };

    template<typename T>
    bool World::HasComponent(Id entity)
    {
        RegisterComponent<T>();
        return HasComponent(entity, T::GetComponentId());
    }

    template <typename T, typename Y>
    bool World::HasRelationship(Id entity)
    {
        RegisterComponent<T>();
        RegisterComponent<Y>();
        return HasComponent(entity, CreateComponentRelationship<T, Y>());
    }

    template <typename T>
    bool World::HasRelationshipWildcard(Id entity)
    {
        RegisterComponent<T>();
        return HasRelationshipWildcardInternal(entity, bits::UnpackA64(T::GetComponentId()));
    }

    template<typename T, typename Y>
    Id World::CreateObserver()
    {
        static_assert(std::is_base_of_v<BaseObserver, T>, "Observers must inherit from BaseObserver");
        static_assert(std::is_base_of_v<Observer<Y>, T>, "Observers must inherit from Observer<Y>");

        auto guard = MaybeLockGuard(m_UpdateMode, &m_ObserverMutex);

        Id observer;
        if (!m_FreeObservers.empty())
        {
            observer = RecycleObserver();
        }
        else
        {
            observer = NewObserver();
        }

        RegisterComponent<Y>();
        m_Observers[Y::GetComponentId()][observer] = std::make_shared<T>();
        return observer;
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
                T::s_ComponentId = bits::Pack64(RecycleEntity(), 0);
            }
            else
            {
                T::s_ComponentId = bits::Pack64(NewEntity(), 0);
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
    std::pair<Id, ComponentMutability::Type> World::CreateComponentUsagePair(ComponentMutability::Type type)
    {
        RegisterComponent<T>();
        return { T::GetComponentId(), type };
    }

    template <typename T, typename Y>
    Relationship World::CreateComponentRelationship()
    {
        static_assert(!std::is_same_v<components::ChildOf, T>, "Child relationships should be created via World::AddChild");
        static_assert(std::is_base_of_v<RelationshipComponent, T>, "Relationship components must inherit from RelationshipComponent.");
        RegisterComponent<T>();
        Relationship relationship;
        relationship.SetId(bits::Pack64(bits::UnpackA64(T::GetComponentId()), bits::UnpackA64(Y::GetComponentId())));
        return relationship;
    }

    template <typename T>
    void CollectComponentId(std::vector<std::pair<Id, ComponentMutability::Type>>& compIds)
    {
        if (!T::IsSingletonComponent())
        {
            compIds.push_back(std::make_pair(T::GetComponentId(), std::is_const<T>() ? ComponentMutability::Immutable : ComponentMutability::Mutable));
        }
    }

    void CollectRelationshipIds(std::vector<std::pair<Id, ComponentMutability::Type>>& compIds, const std::vector<Relationship>& relationships);

    template<typename ...T, typename Func>
    void World::Each(Func&& func, const std::vector<Relationship>& relationships, bool force)
    {
        std::vector<std::pair<Id, ComponentMutability::Type>> compIds = {};
        compIds.reserve(sizeof...(T) + relationships.size());
        (CollectComponentId<T>(compIds), ...);
        std::unordered_map<Id, std::unordered_map<Id, size_t>> argIndexs;
        CollectRelationshipIds(compIds, relationships);

        std::set<Archetype*> archetypes;
        for (auto compId: compIds)
        {
            if (m_ComponentRecords.contains(compId.first))
            {
                auto& compRecord = m_ComponentRecords[compId.first];
                for (const auto& archetypeId : compRecord.archetypeRecords | std::views::keys)
                {
                    auto& archetype = m_Archetypes.at(archetypeId);

                    if (!argIndexs.contains(archetypeId))
                    {
                        auto& compKeys = argIndexs[archetypeId];
                        for (auto comp: compIds)
                        {
                            compKeys.insert(std::make_pair(comp.first, std::ranges::find(archetype.type, comp.first) -
                                                                 archetype.type.begin()));
                        }
                    }

                    bool containsAll = true;
                    for (auto comp: compIds)
                    {
                        containsAll &= std::ranges::contains(archetype.type, comp.first);
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
            if (!archetype->entities.empty())
            {
                Action<T...>::DoAction(archetype->entities, m_SingletonComponents, archetype, func);
            }
        }

        bool hasStaticComps = compIds.size() - relationships.size() != sizeof...(T);
        if (archetypes.empty() && (force || hasStaticComps))
        {
            Action<T...>::DoAction({}, m_SingletonComponents, nullptr, func);
        }
    }

    template <typename ... T, typename Func>
    void World::ChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships)
    {
#if !SPARK_DIST
        std::vector<std::pair<Id, ComponentMutability::Type>> compIds = {};
        compIds.reserve(sizeof...(T) + relationships.size());
        (CollectComponentId<T>(compIds), ...);
        CollectRelationshipIds(compIds, relationships);

        if (!ValidateChildQuery(system, compIds))
        {
            return;
        }
#endif

        std::vector<Relationship> relationshipCopy = relationships;
        relationshipCopy.push_back(CreateChildRelationship(entity));
        Each<T...>(func, relationshipCopy, false);
    }

    template<typename ... T, typename Func>
    void World::RecursiveChildEach(Id entity, BaseSystem *system, Func &&func, const std::vector<Relationship> &relationships)
    {
        auto processChild = [this, system, func, relationships](const std::vector<Id>& children)
        {
            for (const auto& id : children)
            {
                RecursiveChildEach<T...>(id, system, func, relationships);
            }
        };
        ChildEach<>(entity, system, processChild, relationships);
        ChildEach<T...>(entity, system, func, relationships);
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
    Id World::CreateEngineSystem(const std::vector<Relationship>& relationships, const ChildQuery& additionalChildQuery, const std::unordered_set<Id>& dependsOn)
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
        m_PendingEngineSystemCreations.push_back({system, { relationships, additionalChildQuery, dependsOn }});
        if (SPARK_VERIFY(!m_EngineSystems.contains(system)))
        {
            m_EngineSystems.insert(std::make_pair(system, SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }

        return system;
    }

    template <AppSystemConcept T>
    Id World::CreateAppSystem(const std::vector<Relationship>& relationships, const ChildQuery& additionalChildQuery, const std::unordered_set<Id>& dependsOn)
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
        m_PendingAppSystemCreations.push_back({system, { relationships, additionalChildQuery, dependsOn }});
        if (SPARK_VERIFY(!m_AppSystems.contains(system)))
        {
            m_AppSystems.insert(std::make_pair(system, SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }

        return system;
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
