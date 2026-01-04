#pragma once

#include "engine/memory/Arena.h"
#include "Archetype.h"
#include "engine/reflect/TypeResolver.h"
#include "UpdateMode.h"
#include "MaybeLockGuard.h"
#include "Observer.h"
#include "SystemDeclaration.h"
#include "SystemUpdateData.h"
#include <easy/profiler.h>
#include "engine/ecs/components/ParentComponent.h"
#include "ecs_fwd.h"
#include "EntityRecord.h"
#include "ComponentRecord.h"
#include "SceneRecord.h"
#include "SystemRecord.h"

#if SPARK_EDITOR
#include "editor/ui/PropertiesWindow.h"
#endif

namespace se::ecs::components
{
    struct ChildOf;
}

namespace se::ecs
{
    class EngineSystem;
    class AppSystem;
}

template<class C>
concept EngineSystemConcept = std::is_base_of_v<se::ecs::EngineSystem, C>;

template<class C>
concept AppSystemConcept = std::is_base_of_v<se::ecs::AppSystem, C>;

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
    class System;
    template<typename... Cs>
    class SignalActionBuilder;
    class BaseSignal;

    class World
    {
#if SPARK_EDITOR
        friend class editor::ui::PropertiesWindow;
#endif
        friend class System;
        template<typename... Cs>
        friend class Signal;
        template<typename... Cs>
        friend class SignalActionBuilder;

    public:
        World();

        void Init();
        void Update();
        void Render();
        void Shutdown();

        Id LoadScene(const std::string& path);
        void UnloadScene(const Id& scene);
        void SaveScene(const std::string& path);

        Id CreateEntity(const std::string& name);
        Id CreateEntity(const Id& scene,
                        const std::string& name);
        void DestroyEntity(const Id& entity);
        std::vector<Id> GetEntities() const;
        std::vector<Id> GetRootEntities();

        template<typename T>
        T* GetComponent(const Id& entity);

        template<typename T>
        T* AddComponent(const Id& entity);

        void AddChild(const Id& entity,
                      const Id& childEntity);
        void RemoveChild(const Id& entity,
                         const Id& childEntity);
        const std::vector<Id>& GetChildren(const Id& entity) const;
        Id GetParent(const Id& entity) const;

        template<typename T>
        void RemoveComponent(const Id& entity);

        template<typename T>
        T* AddSingletonComponent();

        template<typename T>
        void RemoveSingletonComponent();

        template<typename T>
        T* GetSingletonComponent();

        std::vector<reflect::ObjectBase*> GetSingletonComponents() const;

        template<typename T>
        void RegisterSystem();

        template<AppSystemConcept T>
        void CreateAppSystem();
        void DestroyAppSystem(const Id& id);

        template<EngineSystemConcept T>
        void CreateEngineSystem();
        void DestroyEngineSystem(const Id& id);

        template<typename T>
        void RegisterComponent();
        template<typename T>
        std::pair<Id, ComponentMutability> CreateComponentUsagePair(ComponentMutability type);

        template<typename T>
        bool HasComponent(const Id& entity);

        bool IsChildOf(const Id& entity,
                       const Id& parent) const;

        template<typename T, typename Y>
        Id CreateObserver();
        void DestroyObserver(const Id& id);

        Id CreateScene(const std::string& name);

        const std::string* GetName(uint64_t id) const;
        int32_t* GetFlags(uint64_t id);
        const Id* GetScene(uint64_t id) const;

    private:
        bool IsRunning() const { return m_Running; }

        template<typename Func>
        void Each(const std::vector<ComponentUsage>& components,
                  const std::vector<ComponentUsage>& singletonComponents,
                  Func&& func,
                  bool force);

        void CollectArchetypes(const std::vector<ComponentUsage>& components,
                               std::set<Archetype*>& archetypes);

        struct VariantQueryArchetype
        {
            Archetype* archetype = nullptr;
            ComponentUsage variantUsage = ComponentUsage(NullComponentType::GetComponentId(), ComponentMutability::Immutable);

            bool operator <(const VariantQueryArchetype& rhs) const
            {
                return archetype < rhs.archetype;
            }
        };

        void CollectArchetypesWithVariant(std::vector<ComponentUsage> components,
                                          const ComponentUsage& variantUsage,
                                          std::set<VariantQueryArchetype>& archetypes);

        template<typename Func>
        bool ChildEach(const Id& entity,
                       const System* system,
                       const HeirachyQueryDeclaration& declaration,
                       Func&& func);

        template<typename Func>
        void HeirachyQuery(const Id& child,
                           const System* system,
                           const HeirachyQueryDeclaration& declaration,
                           Func&& func);

        template<typename Func>
        void ParentQuery(const Id& entity,
                         const System* system,
                         const HeirachyQueryDeclaration& declaration,
                         Func&& func);

        template<typename Func>
        bool RecursiveChildEach(const Id& entity,
                                System* system,
                                const HeirachyQueryDeclaration& declaration,
                                Func&& func);

        template<typename Func>
        bool RecurseChildren(const Id& entity,
                             System* system,
                             const HeirachyQueryDeclaration& declaration,
                             Func&& func);

        static bool ValidateHeirachyQuery(const System* system,
                                          const HeirachyQueryDeclaration& declaration);

        void RunOnAllSystems(const std::function<void(const Id&)>& func,
                             const std::vector<std::vector<Id>>& systems,
                             bool parallel,
                             bool processPending);
        void RunOnAllAppSystems(const std::function<void(const Id&)>& func,
                                bool parallel,
                                bool processPending);
        void RunOnAllEngineSystems(const std::function<void(const Id&)>& func,
                                   bool parallel,
                                   bool processPending);

        struct PendingComponent
        {
            Id entity;
            Id comp;
            void* tempData;
        };

        void AddComponentInternal(const PendingComponent& pendingComp);
        void RemoveComponentInternal(const Id& entity,
                                     const Id& comp);

        uint64_t NewSystem();

        static void CreateSystemInternal(std::unordered_map<Id, SystemRecord>& systemMap,
                                         const Id& system,
                                         const SystemDeclaration& pendingSystem);
        static void DestroySystemInternal(std::unordered_map<Id, SystemRecord>& systemMap,
                                          const Id& system);

        Id NewObserver();
        Id RecycleObserver();

        bool HasComponent(const Id& entity,
                          const Id& component);
        void* GetComponent(const Id& entity,
                           const Id& component);

        uint64_t NewEntity();
        uint64_t RecycleEntity();
        void DestroyEntityInternal(const Id& entity);
        size_t MoveEntity(const Id& entity,
                          Archetype* archetype,
                          size_t entityIdx,
                          Archetype* nextArchetype);

        Archetype* GetArchetype(const Type& type,
                                bool createIfMissing);
        bool HasArchetype(const Type& type) const;
        void CreateArchetype(const Type& type);
        Archetype* GetNextArchetype(Archetype* archetype,
                                    Id component,
                                    bool add,
                                    bool createIfMissing);

        void ProcessAllPending();
        void ProcessPendingComponents();
        void ProcessPendingSystems();
        static void ProcessPendingSystems(std::vector<std::pair<Id, SystemDeclaration>>& pendingCreations,
                                          std::vector<Id>& pendingDeletions,
                                          std::unordered_map<Id, SystemRecord>& systemRecords,
                                          std::vector<std::vector<Id>>& systemUpdateGroups);
        static void RebuildSystemUpdateGroups(std::vector<std::vector<Id>>& updateGroups,
                                              std::unordered_map<Id, SystemRecord>& systems);
        void ProcessPendingAppSystems();
        void ProcessPendingEngineSystems();
        void ProcessPendingEntityDeletions();

        void AddPendingSignal(BaseSignal* signal);
        void OnSignalDestroyed(BaseSignal* signal);

        bool m_Running = false;

        std::unordered_map<Id, Archetype> m_Archetypes = { };
        std::unordered_map<Type, Id> m_ArchetypeTypeLookup = { };
        std::unordered_map<Id, EntityRecord> m_EntityRecords = { };
        std::unordered_map<Id, SceneRecord> m_SceneRecords = { };
        std::unordered_map<Id, ComponentRecord> m_ComponentRecords = { };
        std::unordered_map<Id, reflect::ObjectBase*> m_SingletonComponents = { };
        std::unordered_map<Id, SystemRecord> m_AppSystems = { };
        std::unordered_map<Id, SystemRecord> m_EngineSystems = { };
        std::unordered_map<Id, std::unordered_map<Id, std::shared_ptr<BaseObserver>>> m_Observers;

        uint64_t m_EntityCounter = 1;
#if SPARK_EDITOR
        bool m_EntitiesChangedThisFrame = false;
#endif
        std::vector<uint64_t> m_FreeEntities = { };
        uint32_t m_ComponentCounter = 0;
        uint64_t m_SystemCounter = 1;
        uint64_t m_ArchetypeCounter = 0;
        uint64_t m_ObserverCounter = 1;
        std::vector<Id> m_FreeObservers = { };
        Id m_DefaultScene = s_InvalidEntity;

        std::vector<std::vector<Id>> m_AppSystemUpdateGroups = { };
        std::vector<std::vector<Id>> m_EngineSystemUpdateGroups = { };
        UpdateMode m_UpdateMode = UpdateMode::SingleThreaded;
        std::mutex m_EntityMutex = { };
        std::mutex m_ComponentMutex = { };
        std::mutex m_SystemMutex = { };
        std::mutex m_ObserverMutex = { };

        std::vector<Id> m_PendingEntityDeletions = { };
        std::vector<PendingComponent> m_PendingComponentCreations = { };
        std::vector<std::pair<Id, Id>> m_PendingComponentDeletions = { };

        std::vector<std::pair<Id, SystemDeclaration>> m_PendingAppSystemCreations = { };
        std::vector<Id> m_PendingAppSystemDeletions = { };
        std::vector<std::pair<Id, SystemDeclaration>> m_PendingEngineSystemCreations = { };
        std::vector<Id> m_PendingEngineSystemDeletions = { };
        memory::Arena m_TempStore = memory::Arena(20000000); // cleared after all pending creations/deletions
        bool m_ClearingTempObjects = { };

        std::vector<BaseSignal*> m_PendingSignals = { };

        struct IdMetaData
        {
            std::string name = {};
            int32_t flags = {};
            Id scene = {};
        };

        std::unordered_map<uint64_t, IdMetaData> m_IdMetaMap;
    };

    template<typename T>
    bool World::HasComponent(const Id& entity)
    {
        return HasComponent(entity, T::GetComponentId());
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

        m_Observers[Y::GetComponentId()][observer] = std::make_shared<T>();
        return observer;
    }

    template<typename T>
    T* World::GetComponent(const Id& entity)
    {
        if (!SPARK_VERIFY(!IsRunning()))
        {
            return nullptr;
        }

        return static_cast<T*>(GetComponent(entity, T::GetComponentId()));
    }

    template<typename T>
    void World::RegisterComponent()
    {
        if (T::s_ComponentId == static_cast<uint64_t>(0))
        {
            uint64_t id = m_ComponentCounter++;

            reflect::Type* type = reflect::TypeResolver<T>::get();
            m_IdMetaMap[id].name = type->name;

            T::s_ComponentId = id;
        }

        if (!T::IsSingletonComponent() && !m_ComponentRecords.contains(T::GetComponentId()))
        {
            m_ComponentRecords.insert(std::make_pair(T::GetComponentId(),
                                                     ComponentRecord
                                                     {
                                                         .type = reflect::ClassResolver<T>::get(),
                                                         .archetypeRecords = { },
                                                     }));
        }
    }

    template<typename T>
    std::pair<Id, ComponentMutability> World::CreateComponentUsagePair(ComponentMutability type)
    {
        return { T::GetComponentId(), type };
    }

    template<typename Func>
    void World::Each(const std::vector<ComponentUsage>& components,
                     const std::vector<ComponentUsage>& singletonComponents,
                     Func&& func,
                     bool force)
    {
        EASY_BLOCK("World::Each");

        EASY_BLOCK("Collect Archetypes");
        std::set<Archetype*> archetypes = { };
        CollectArchetypes(components, archetypes);
        EASY_END_BLOCK

        EASY_BLOCK("Collect Singletons");
        SystemUpdateData updateData = { };
        for (const auto& compUsage: singletonComponents)
        {
            updateData.AddSingletonComponent(compUsage.id, m_SingletonComponents.at(compUsage.id), compUsage.mutability);
        }
        EASY_END_BLOCK

        EASY_BLOCK("Run on Archetypes");
        for (auto* archetype: archetypes)
        {
            if (!archetype->entities.empty())
            {
                updateData.ClearEntityData();
                updateData.SetEntities(archetype->entities);
                for (const auto& compUsage: components)
                {
                    const auto& compInfo = m_ComponentRecords[compUsage.id];
                    const ArchetypeComponentKey& key = compInfo.archetypeRecords.at(archetype->id);
                    updateData.AddComponentArray(compUsage.id, archetype->components.at(key).Data(), compUsage.mutability);
                }

                func(updateData);
            }
        }
        EASY_END_BLOCK

        EASY_BLOCK("Run on empty");
        bool hasStaticComps = !singletonComponents.empty();
        if (archetypes.empty() && (force || hasStaticComps))
        {
            std::vector<Id> empty;
            updateData.SetEntities(empty);

            for (const auto& compUsage: components)
            {
                updateData.AddComponentArray(compUsage.id, nullptr, compUsage.mutability);
            }
            func(updateData);
        }
        EASY_END_BLOCK
    }

    template<typename Func>
    bool World::ChildEach(const Id& entity,
                          [[maybe_unused]] const System* system,
                          const HeirachyQueryDeclaration& declaration,
                          Func&& func)
    {
        EASY_BLOCK("World::ChildEach");;

        if (!HasComponent<components::ParentComponent>(entity))
        {
            return false;
        }

#if !SPARK_DIST
        if (!ValidateHeirachyQuery(system, declaration))
        {
            return false;
        }
#endif
        bool hasNullVariant = false;
        std::set<Id> variantTypes = { };
        if (declaration.variantComponentUsage.type_hash != 0)
        {
            for (const auto& type: declaration.variantComponentUsage.components)
            {
                if (type == s_InvalidEntity)
                {
                    hasNullVariant = true;
                    continue;
                }
                variantTypes.insert(type);
            }
        }

        SystemUpdateData updateData = { };
        for (const auto& compUsage: declaration.singletonComponentUsage)
        {
            updateData.AddSingletonComponent(compUsage.id, m_SingletonComponents.at(compUsage.id), compUsage.mutability);
        }

        for (const auto& child: GetChildren(entity))
        {
            updateData.ClearEntityData();
            std::vector entities = { child };
            updateData.SetEntities(entities);

            bool shouldSkip = false;
            for (const auto& comp: declaration.componentUsage)
            {
                if (!HasComponent(child, comp.id))
                {
                    shouldSkip = true;
                    break;
                }

                updateData.AddComponentArray(comp.id, GetComponent(child, comp.id), comp.mutability);
            }

            bool hasAnyVariant = hasNullVariant || declaration.variantComponentUsage.components.empty();
            for (const auto& comp: declaration.variantComponentUsage.components)
            {
                if (!HasComponent(child, comp))
                {
                    continue;
                }

                hasAnyVariant = true;
                updateData.AddVariantComponentArray(comp.id,
                                                    GetComponent(child, comp),
                                                    declaration.variantComponentUsage.mutability);
                break;
            }

            if (shouldSkip || !hasAnyVariant)
            {
                continue;
            }

            if (func(updateData))
            {
                return true;
            }
        }

        return false;
    }

    template<typename Func>
    void World::HeirachyQuery(const Id& child,
                              [[maybe_unused]] const System* system,
                              const HeirachyQueryDeclaration& declaration,
                              Func&& func)
    {
#if !SPARK_DIST
        if (!ValidateHeirachyQuery(system, declaration))
        {
            return;
        }
#endif

        SystemUpdateData updateData = { };
        for (const auto& compUsage: declaration.singletonComponentUsage)
        {
            updateData.AddSingletonComponent(compUsage.id, m_SingletonComponents.at(compUsage.id), compUsage.mutability);
        }
        updateData.SetEntities({ child });
        for (const auto& comp: declaration.componentUsage)
        {
            updateData.AddComponentArray(comp.id,
                                         GetComponent(child, comp.id),
                                         comp.mutability);
        }

        bool didFindVariant = declaration.variantComponentUsage.components.empty();
        if (!didFindVariant)
        {
            for (const auto& comp: declaration.variantComponentUsage.components)
            {
                if (HasComponent(child, comp))
                {
                    updateData.AddComponentArray(comp,
                                                 GetComponent(comp, child),
                                                 declaration.variantComponentUsage.mutability);
                    didFindVariant = true;
                    break;
                }
            }
        }
        if (!didFindVariant)
        {
            updateData.AddComponentArray(s_InvalidEntity,
                                         nullptr,
                                         ComponentMutability::Immutable);
        }

        func(updateData);
    }

    template<typename Func>
    void World::ParentQuery(const Id& entity,
                            const System* system,
                            const HeirachyQueryDeclaration& declaration,
                            Func&& func)
    {
        Id parent = GetParent(entity);
        if (parent == s_InvalidEntity)
        {
            return;
        }

        HeirachyQuery(parent, system, declaration, func);
    }

    template<typename Func>
    bool World::RecursiveChildEach(const Id& entity,
                                   System* system,
                                   const HeirachyQueryDeclaration& declaration,
                                   Func&& func)
    {
        EASY_BLOCK("World::RecursiveChildEach");

        if (RecurseChildren(entity, system, declaration, func))
        {
            return true;
        }

        return false;
    }

    template<typename Func>
    bool World::RecurseChildren(const Id& entity,
                                System* system,
                                const HeirachyQueryDeclaration& declaration,
                                Func&& func)
    {
        if (HasComponent<components::ParentComponent>(entity))
        {
            for (const auto& child: m_EntityRecords.at(entity).children)
            {
                if (RecurseChildren(child, system, declaration, func))
                {
                    return true;
                }
            }

            if (ChildEach(entity, system, declaration, func))
            {
                return true;
            }
        }

        return false;
    }

    template<typename T>
    T* World::AddComponent(const Id& entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);

        if (!SPARK_VERIFY(!HasComponent<T>(entity)))
        {
            return nullptr;
        }

        T* tempComp = m_TempStore.Alloc<T>();
        m_PendingComponentCreations.emplace_back(PendingComponent { .entity = entity, .comp = T::GetComponentId(), .tempData = tempComp });
        SPARK_ASSERT(m_PendingComponentCreations.back().comp.name != nullptr);
        return tempComp;
    }

    template<typename T>
    void World::RemoveComponent(const Id& entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        if (!SPARK_VERIFY(HasComponent<T>(entity)))
        {
            return;
        }
        m_PendingComponentDeletions.push_back(std::make_pair(entity, T::GetComponentId()));
    }


    template<EngineSystemConcept T>
    void World::CreateEngineSystem()
    {
        SystemDeclaration systemReg = T::GetSystemDeclaration();
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        m_PendingEngineSystemCreations.push_back({ T::GetSystemId(), systemReg });
        if (SPARK_VERIFY(!m_EngineSystems.contains(T::GetSystemId())))
        {
            m_EngineSystems.insert(std::make_pair(T::GetSystemId(), SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
        }
    }

    template<typename T>
    void World::RegisterSystem()
    {
        if (T::s_SystemId == static_cast<uint64_t>(0))
        {
            uint64_t id = NewSystem();
            reflect::Type* type = reflect::TypeResolver<T>::get();
            m_IdMetaMap[id].name = type->name;

            T::s_SystemId = id;
        }
    }

    template<AppSystemConcept T>
    void World::CreateAppSystem()
    {
        SystemDeclaration systemReg = T::GetSystemDeclaration();
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        m_PendingAppSystemCreations.push_back({ T::GetSystemId(), systemReg });
        if (SPARK_VERIFY(!m_AppSystems.contains(T::GetSystemId())))
        {
            m_AppSystems.insert(std::make_pair(T::GetSystemId(), SystemRecord { reflect::ClassResolver<T>::get(), nullptr }));
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

    template<typename T>
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
