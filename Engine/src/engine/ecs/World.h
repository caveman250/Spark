#pragma once

#include "engine/memory/Arena.h"
#include "Archetype.h"
#include "engine/reflect/TypeResolver.h"
#include "Action.h"
#include "UpdateMode.h"
#include "MaybeLockGuard.h"
#include "Observer.h"
#include "Relationship.h"
#include "SystemCreationInfo.h"
#include "engine/profiling/Profiler.h"
#include "engine/string/String.h"
#include "engine/ecs/components/ParentComponent.h"

#if SPARK_EDITOR
#include "ui/PropertiesWindow.h"
#endif

namespace se::ecs::components
{
    struct ChildOf;
}

namespace se::ecs
{
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
    constexpr uint64_t s_InvalidEntity = 0;

    struct EntityRecord
    {
        Archetype* archetype;
        size_t entity_idx; // index/row of entity in component data
        Id parent = s_InvalidEntity;
        std::vector<Id> children = {};
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
        reflect::Class* type = nullptr;
        BaseSystem* instance = nullptr;
    };

    class World
    {
#if SPARK_EDITOR
        friend class editor::ui::PropertiesWindow;
#endif
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

        Id CreateEntity(const String& name, bool editorOnly = false);
        void DestroyEntity(Id entity);
        std::vector<Id> GetEntities() const;
        std::vector<Id> GetRootEntities();

        template<typename T>
        T* GetComponent(Id entity);

        template<typename T>
        T* AddComponent(Id entity);

        void AddRelationship(Id entity, const Relationship& relationship);
        void RemoveRelationship(Id entity, const Relationship& relationship);
        void AddChild(Id entity, Id childEntity);
        void RemoveChild(Id entity, Id childEntity);
        const std::vector<Id>& GetChildren(Id entity) const;
        Id GetParent(Id entity) const;

        template<typename T>
        void RemoveComponent(Id entity);

        template <typename T>
        T* AddSingletonComponent();

        template <typename T>
        void RemoveSingletonComponent();

        template <typename T>
        T* GetSingletonComponent();

        std::vector<reflect::ObjectBase*> GetSingletonComponents();

        template <AppSystemConcept T>
        Id CreateAppSystem(const SystemCreationInfo& reg_info);
        void DestroyAppSystem(Id id);

        template <EngineSystemConcept T>
        Id CreateEngineSystem(const SystemCreationInfo& reg_info);
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

        const String* GetName(uint64_t id);
        int32_t* GetFlags(uint64_t id);

    private:
        bool IsRunning() const { return m_Running; }

        template<typename ...T, typename Func>
        void Each(Func&& func, const std::vector<Relationship>& relationships, bool force);
        std::set<Archetype*> CollectArchetypes(const std::vector<std::pair<Id, ComponentMutability::Type>>& compIds);

        template<typename ...T, typename Func>
        bool ChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships);

        template<typename ...T>
        bool VariantChildEach(const Id& entity, BaseSystem* system, const std::function<bool(std::variant<T*...>)>& func);

        template <typename T, typename... Ts>
        void VariantChildEachImpl(const Id& entity, const std::function<bool(std::variant<Ts*...>)>& func, bool& ret);

        template<typename ...T, typename Func>
        bool RecursiveChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships);

        template<typename ...T, typename Func>
        bool RecurseChildren(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships);

        bool ValidateChildQuery(BaseSystem* system, const std::vector<std::pair<Id, ComponentMutability::Type>>& requestedComponents);
        Relationship CreateChildRelationship(Id entity);
        bool HasRelationshipWildcardInternal(Id entity, uint32_t lhs);

        void RegisterRelationship(uint64_t id);

        void RunOnAllSystems(const std::function<void(const Id&)>& func, const std::vector<std::vector<Id>>& systems, bool parallel, bool processPending);
        void RunOnAllAppSystems(const std::function<void(const Id&)>& func, bool parallel, bool processPending);
        void RunOnAllEngineSystems(const std::function<void(const Id&)>& func, bool parallel, bool processPending);

        struct PendingComponent
        {
            Id entity;
            Id comp;
            void* tempData;
        };

        void AddComponentInternal(const PendingComponent& pendingComp);
        void RemoveComponentInternal(Id entity, Id comp);

        uint64_t NewSystem();
        uint64_t RecycleSystem();

        static void CreateSystemInternal(std::unordered_map<Id, SystemRecord>& systemMap, Id system, const SystemCreationInfo& pendingSystem);
        static void DestroySystemInternal(std::unordered_map<Id, SystemRecord>& systemMap, std::vector<Id>& freeSystems, Id system);

        Id NewObserver();
        Id RecycleObserver();

        bool HasComponent(Id entity, Id component);
        void* GetComponent(Id entity, Id component);

        uint32_t NewEntity();
        uint32_t RecycleEntity();
        void DestroyEntityInternal(Id entity);
        size_t MoveEntity(Id entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type, bool createIfMissing);
        bool HasArchetype(const Type& type) const;
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype, Id component, bool add, bool createIfMissing);

        void ProcessAllPending();
        void ProcessPendingComponents();
        void ProcessPendingSystems();
        static void ProcessPendingSystems(std::vector<std::pair<Id, SystemCreationInfo>>& pendingCreations,
                                    std::vector<Id>& pendingDeletions,
                                    std::unordered_map<Id, SystemRecord>& systemRecords,
                                    std::vector<std::vector<Id>>& systsemUpdateGroups,
                                    std::vector<Id>& freeSystems);
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
        std::unordered_map<Id, reflect::ObjectBase*> m_SingletonComponents = {};
        std::unordered_map<Id, SystemRecord> m_AppSystems = {};
        std::unordered_map<Id, SystemRecord> m_EngineSystems = {};
        std::unordered_map<Id, std::unordered_map<Id, std::shared_ptr<BaseObserver>>> m_Observers;

        uint32_t m_EntityCounter = 1;
#if SPARK_EDITOR
        bool m_EntitiesChangedThisFrame = false;
#endif
        std::vector<uint32_t> m_FreeEntities = {};
        uint64_t m_SystemCounter = 0;
        std::vector<Id> m_FreeSystems = {};
        uint64_t m_ArchetypeCounter = 0;
        uint64_t m_ObserverCounter = 0;
        std::vector<Id> m_FreeObservers = {};

        std::vector<std::vector<Id>> m_AppSystemUpdateGroups = {};
        std::vector<std::vector<Id>> m_EngineSystemUpdateGroups = {};
        UpdateMode::Type m_UpdateMode = UpdateMode::SingleThreaded;
        std::mutex m_EntityMutex = {};
        std::mutex m_ComponentMutex = {};
        std::mutex m_SystemMutex = {};
        std::mutex m_ObserverMutex = {};

        std::vector<Id> m_PendingEntityDeletions = {};
        std::vector<PendingComponent> m_PendingComponentCreations = {};
        std::vector<std::pair<Id, Id>> m_PendingComponentDeletions = {};

        std::vector<std::pair<Id, SystemCreationInfo>> m_PendingAppSystemCreations = {};
        std::vector<Id> m_PendingAppSystemDeletions = {};
        std::vector<std::pair<Id, SystemCreationInfo>> m_PendingEngineSystemCreations = {};
        std::vector<Id> m_PendingEngineSystemDeletions = {};
        memory::Arena m_TempStore = memory::Arena(2000000); // cleared after all pending creations/deletions
        bool m_ClearingTempObjects = {};

        std::vector<BaseSignal*> m_PendingSignals = {};

        struct IdMetaData
        {
            String name;
            int32_t flags;
        };
        std::unordered_map<uint64_t, IdMetaData> m_IdMetaMap;
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
        if (!SPARK_VERIFY(!IsRunning()))
        {
            return nullptr;
        }

        RegisterComponent<T>();
        return static_cast<T*>(GetComponent(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::RegisterComponent()
    {
        if (T::s_ComponentId == static_cast<uint64_t>(0))
        {
            uint64_t id;
            if (!m_FreeEntities.empty())
            {
                 id = bits::Pack64(RecycleEntity(), 0);
            }
            else
            {
                id = bits::Pack64(NewEntity(), 0);
            }

#if !SPARK_DIST
            reflect::Type* type = reflect::TypeResolver<T>::get();
            m_IdMetaMap[id].name = type->name;
#endif
            T::s_ComponentId = id;
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
        PROFILE_SCOPE("World::Each")
        std::vector<std::pair<Id, ComponentMutability::Type>> compIds = {};
        compIds.reserve(sizeof...(T) + relationships.size());
        (CollectComponentId<T>(compIds), ...);
        CollectRelationshipIds(compIds, relationships);

        std::set<Archetype*> archetypes = CollectArchetypes(compIds);

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
    bool World::ChildEach(Id entity, BaseSystem* system, Func&& func, const std::vector<Relationship>& relationships)
    {
        PROFILE_SCOPE("World::ChildEach");

        std::vector<std::pair<Id, ComponentMutability::Type>> compIds = {};
        compIds.reserve(sizeof...(T) + relationships.size());
        (CollectComponentId<T>(compIds), ...);
        CollectRelationshipIds(compIds, relationships);

#if !SPARK_DIST
        if (!ValidateChildQuery(system, compIds))
        {
            return false;
        }
#endif

        compIds.push_back(std::make_pair(CreateChildRelationship(entity).GetId(), ComponentMutability::Immutable));
        std::set<Archetype*> archetypes = CollectArchetypes(compIds);
        for (auto* archetype: archetypes)
        {
            if (!archetype->entities.empty())
            {
                if (Action<T...>::DoBoolAction(archetype->entities, m_SingletonComponents, archetype, func))
                {
                    return true;
                }
            }
        }

        return false;
    }

    template <typename T, typename... Ts>
    void World::VariantChildEachImpl(const Id& entity, const std::function<bool(std::variant<Ts*...>)>& func, bool& ret)
    {
        if (!ret && HasComponent<T>(entity))
        {
            ret = func(static_cast<T*>(GetComponent(entity, T::GetComponentId())));
        }
    }

    template<typename... T>
    bool World::VariantChildEach(const Id& entity, BaseSystem* system, const std::function<bool(std::variant<T*...>)>& func)
    {
        PROFILE_SCOPE("World::VariantChildEach");

#if !SPARK_DIST
        std::vector<std::pair<Id, ComponentMutability::Type>> compIds = {};
        compIds.reserve(sizeof...(T));
        (CollectComponentId<T>(compIds), ...);

        if (!ValidateChildQuery(system, compIds))
        {
            return false;
        }
#endif

        for (const auto& child : GetChildren(entity))
        {
            bool ret = false;
            (VariantChildEachImpl<T>(child, func, ret), ...);
            if (ret)
            {
                return ret;
            }
        }

        return false;
    }

    template<typename ... T, typename Func>
    bool World::RecursiveChildEach(Id entity, BaseSystem *system, Func &&func, const std::vector<Relationship> &relationships)
    {
        PROFILE_SCOPE("World::RecursiveChildEach");

        if (RecurseChildren<T...>(entity, system, func, relationships))
        {
            return true;
        }

        return false;
    }

    template<typename... T, typename Func>
    bool World::RecurseChildren(Id entity, BaseSystem *system, Func &&func, const std::vector<Relationship> &relationships)
    {
        if (HasComponent<components::ParentComponent>(entity))
        {
            for (const auto& child : m_EntityRecords.at(entity).children)
            {
                if (RecurseChildren<T...>(child, system, func, relationships))
                {
                    return true;
                }
            }

            if (ChildEach<T...>(entity, system, func, relationships))
            {
                return true;
            }
        }

        return false;
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
        SPARK_ASSERT(m_PendingComponentCreations.back().comp.name != nullptr);
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
    Id World::CreateEngineSystem(const SystemCreationInfo& reg_info)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        uint64_t system;
        if (!m_FreeSystems.empty())
        {
            system = RecycleSystem();
        }
        else
        {
            system = NewSystem();
        }
        m_IdMetaMap[system] = { reg_info.name, 0 };
        m_PendingEngineSystemCreations.push_back({system, reg_info });
        if (SPARK_VERIFY(!m_EngineSystems.contains(system)))
        {
            m_EngineSystems.insert(std::make_pair(system, SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }

        return system;
    }

    template <AppSystemConcept T>
    Id World::CreateAppSystem(const SystemCreationInfo& reg_info)
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
        m_IdMetaMap[system] = {reg_info.name, 0};
        m_PendingAppSystemCreations.push_back({system, reg_info });
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
