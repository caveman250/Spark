#include "World.h"

#include <engine/container_util/MapUtil.h>

#include "Signal.h"
#include "components/ParentComponent.h"
#include "components/RootComponent.h"
#include "engine/reflect/Reflect.h"
#include "engine/ecs/System.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "relationships/ChildOf.h"
#include "engine/bits/FlagUtil.h"
#include "engine/threads/ParallelForEach.h"

namespace se::ecs
{
    Id World::CreateEntity(const String& name, bool editorOnly)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_EntityMutex);
        uint32_t entityId;

        if (!m_FreeEntities.empty())
        {
            entityId = RecycleEntity();
        }
        else
        {
            entityId = NewEntity();
        }

        if (entityId == s_InvalidEntity)
        {
            SPARK_ASSERT(false, "Max number of entities reached.");
            return s_InvalidEntity;
        }

        int32_t flags = 0;
        if (editorOnly)
        {
            bits::SetFlag<IdFlags>(flags, IdFlags::Editor);
        }
#if SPARK_EDITOR
        else
        {
            m_EntitiesChangedThisFrame = true;
        }
#endif

        uint64_t packedId = bits::Pack64(entityId, 0);
        m_IdMetaMap[packedId] = {name, flags};
        auto emptyArchetype = GetArchetype({}, true);
        SPARK_ASSERT(!m_EntityRecords.contains(packedId));
        auto result = m_EntityRecords.insert(std::make_pair(packedId, EntityRecord
                                              {
                                                  .archetype = emptyArchetype,
                                                  .entity_idx = 0 // not valid for null archetype
                                              }));

        SPARK_ASSERT(result.second);

        AddComponent<components::RootComponent>(packedId);
        [[maybe_unused]] auto& record = m_EntityRecords.at(packedId);
        SPARK_ASSERT(record.archetype);

        return packedId;
    }

    void World::DestroyEngineSystem(Id id)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        m_PendingEngineSystemDeletions.push_back(id);
    }

    std::vector<reflect::ObjectBase*> World::GetSingletonComponents()
    {
        return util::ToValueArray<reflect::ObjectBase*>(m_SingletonComponents);
    }

    void World::DestroyAppSystem(Id id)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_SystemMutex);
        m_PendingAppSystemDeletions.push_back(id);
    }

    uint32_t World::NewEntity()
    {
        return m_EntityCounter++;
    };

    uint32_t World::RecycleEntity()
    {
        const auto entity = m_FreeEntities.back();
        m_FreeEntities.pop_back();

        return entity;
    }

    void* World::GetComponent(Id entity, Id component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        auto compInfo = m_ComponentRecords[component];
        if (!compInfo.archetypeRecords.contains(archetype->id))
        {
            // check pending comps
            for (const auto& pendingComp: m_PendingComponentCreations)
            {
                if (pendingComp.entity == entity && pendingComp.comp == component)
                {
                    return pendingComp.tempData;
                }
            }

            SPARK_ASSERT(false);
            return nullptr;
        }
        ArchetypeComponentKey& a_record = compInfo.archetypeRecords[archetype->id];
        return archetype->components[a_record].GetComponent(record.entity_idx);
    }

    void World::DestroyEntityInternal(Id entity)
    {
        if (entity == s_InvalidEntity)
        {
            return;
        }

#if SPARK_EDITOR
        auto entityFlags = m_IdMetaMap[entity].flags;
        if (!bits::GetFlag<ecs::IdFlags>(entityFlags, ecs::IdFlags::Editor))
        {
            m_EntitiesChangedThisFrame = true;
        }
#endif

        auto& record = m_EntityRecords.at(entity);

        auto safeCopy = record.children;
        for (const auto& child: safeCopy)
        {
            DestroyEntityInternal(child);
        }

        safeCopy = record.archetype->type;
        for (const auto& comp: safeCopy)
        {
            RemoveComponentInternal(entity, comp);
        }

        Id parentId = record.parent;
        if (parentId != s_InvalidEntity)
        {
            auto& childrenRecord = m_EntityRecords[parentId].children;
            std::erase(childrenRecord, entity);
        }

        std::erase(record.archetype->entities, entity);
        m_EntityRecords.erase(entity);
        m_FreeEntities.push_back(bits::UnpackA64(entity));
    }

    bool World::HasRelationshipWildcardInternal(Id entity, uint32_t lhs)
    {
        EntityRecord& record = m_EntityRecords.at(entity);

        if (Archetype* archetype = record.archetype)
        {
            for (const auto& id: archetype->type)
            {
                uint32_t typeRhs = bits::UnpackB64(id);
                if (typeRhs == 0)
                {
                    continue; // not a relationship
                }
                uint32_t typeLhs = bits::UnpackA64(id);
                if (typeLhs == lhs)
                {
                    return true;
                }
            }
        }

        // need to check pending comps.
        for (const auto& pendingComp: m_PendingComponentCreations)
        {
            if (pendingComp.entity == entity)
            {
                auto id = pendingComp.comp;
                uint32_t typeRhs = bits::UnpackB64(id);
                if (typeRhs == 0)
                {
                    continue; // not a relationship
                }
                uint32_t typeLhs = bits::UnpackA64(id);
                if (typeLhs == lhs)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool World::HasComponent(Id entity, Id component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        if (!archetype)
        {
            return false;
        }
        auto compInfo = m_ComponentRecords[component];
        if (compInfo.archetypeRecords.contains(archetype->id))
        {
            return true;
        }

        // need to check pending comps.
        for (const auto& pendingComp: m_PendingComponentCreations)
        {
            if (pendingComp.entity == entity && pendingComp.comp == component)
            {
                return true;
            }
        }

        return false;
    }

    bool World::HasArchetype(const Type& type) const
    {
        return m_ArchetypeTypeLookup.contains(type);
    }

    void World::CreateArchetype(const Type& type)
    {
        Archetype archetype = {
            .id = m_ArchetypeCounter++,
            .type = type,
            .entities = {},
            .components = {},
            .edges = {}
        };

        archetype.components.reserve(type.size());
        for (const auto& compType: type)
        {
            SPARK_ASSERT(compType.name != nullptr);
            auto& compInfo = m_ComponentRecords[compType];
            archetype.components.emplace_back(compInfo.type->size);
        }

        m_Archetypes.insert(std::make_pair(archetype.id, archetype));
        m_ArchetypeTypeLookup.insert(std::make_pair(type, archetype.id));

        for (size_t i = 0; i < type.size(); ++i)
        {
            auto& compInfo = m_ComponentRecords[type[i]];
            compInfo.archetypeRecords.insert(std::make_pair(archetype.id, i));
        }
    }

    size_t World::MoveEntity(Id entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype)
    {
        //need to update all entity records when an archetype is updated ew.
        std::vector<std::pair<Id, uint8_t*>> compData;
        for (size_t i = 0; i < archetype->components.size(); ++i)
        {
            Id id = archetype->type[i];
            uint8_t* data = archetype->components[i].GetComponent(entityIdx);

            compData.emplace_back(std::make_pair(id, data));
        }

        // copy to new archetype
        std::set<Id> componentsAccountedFor = {};
        for (auto& data: compData)
        {
            auto compInfo = m_ComponentRecords[data.first];
            if (!compInfo.archetypeRecords.contains(nextArchetype->id))
            {
                // not part of new archetype
                continue;
            }
            ArchetypeComponentKey& aRecord = compInfo.archetypeRecords[nextArchetype->id];
            nextArchetype->components[aRecord].AddComponent(data.second);

            componentsAccountedFor.insert(data.first);
        }

        // add missing components
        for (size_t i = 0; i < nextArchetype->components.size(); ++i)
        {
            if (!componentsAccountedFor.contains(nextArchetype->type[i]))
            {
                auto compInfo = m_ComponentRecords[nextArchetype->type[i]];
                std::vector<uint8_t> tempCompData(compInfo.type->size);
                nextArchetype->components[i].AddComponent(tempCompData.data());
            }
        }

        // remove from og archetype
        for (size_t i = 0; i < archetype->components.size(); ++i)
        {
            archetype->components[i].RemoveComponent(entityIdx);
        }

        for (const auto& otherEntity: archetype->entities)
        {
            auto& otherRecord = m_EntityRecords.at(otherEntity);
            if (otherRecord.entity_idx > entityIdx)
            {
                otherRecord.entity_idx--;
            }
        }

        std::erase(archetype->entities, entity);
        nextArchetype->entities.push_back(entity);

        if (archetype->entities.empty() && !archetype->type.empty())
        {
            for (auto& otherArchetype: m_Archetypes | std::views::values)
            {
                for (auto& edge : otherArchetype.edges | std::views::values)
                {
                    if (edge.add == archetype)
                    {
                        edge.add = nullptr;
                    }

                    if (edge.remove == archetype)
                    {
                        edge.remove = nullptr;
                    }
                }
            }

            for (size_t i = 0; i < archetype->type.size(); ++i)
            {
                auto& compInfo = m_ComponentRecords[archetype->type[i]];
                compInfo.archetypeRecords.erase(archetype->id);
            }

            m_ArchetypeTypeLookup.erase(archetype->type);
            m_Archetypes.erase(archetype->id);
        }

        if (nextArchetype->components.size() > 0)
        {
            return nextArchetype->components[0].m_Count - 1;
        }

        return 0;
    }

    World::World()
    {
        CreateArchetype({});
    }

    Archetype* World::GetNextArchetype(Archetype* archetype, Id component, bool add, bool createIfMissing)
    {
        if (!archetype->edges.contains(component))
        {
            archetype->edges.insert(std::make_pair(component, ArchetypeEdge()));
        }
        auto& edge = archetype->edges.at(component);
        if (!edge.add && !edge.remove)
        {
            if (add)
            {
                auto addType = archetype->type;
                if (!std::ranges::contains(addType, component))
                {
                    addType.push_back(component);
                }
                edge.add = GetArchetype(addType, createIfMissing);
            }
            else
            {
                auto removeType = archetype->type;
                auto [first, last] = std::ranges::remove(removeType, component);
                removeType.erase(first, last);
                edge.remove = GetArchetype(removeType, createIfMissing);
            }
        }
        auto ret = add ? edge.add : edge.remove;
        return ret;
    }

    void World::ProcessAllPending()
    {
         if (!m_PendingComponentCreations.empty() || !m_PendingComponentDeletions.empty() ||
             !m_PendingAppSystemCreations.empty() || !m_PendingAppSystemDeletions.empty() ||
             !m_PendingEngineSystemCreations.empty() || !m_PendingEngineSystemDeletions.empty() ||
             !m_PendingEntityDeletions.empty())
         {
            ProcessPendingComponents();
            ProcessPendingSystems();
            ProcessPendingEntityDeletions();

            if (m_PendingComponentCreations.empty() && m_PendingComponentDeletions.empty() &&
                m_PendingAppSystemCreations.empty() && m_PendingAppSystemDeletions.empty() &&
                m_PendingEngineSystemCreations.empty() && m_PendingEngineSystemDeletions.empty() &&
                m_PendingEntityDeletions.empty())
            {
                m_TempStore.Reset();
            }
         }
    }

    Archetype* World::GetArchetype(const Type& type, bool createIfMissing)
    {
        if (!HasArchetype(type))
        {
            if (!createIfMissing)
            {
                return nullptr;
            }

            CreateArchetype(type);
        }

        auto lookup = m_ArchetypeTypeLookup.at(type);
        auto ret = &m_Archetypes.at(lookup);
        SPARK_ASSERT(ret);
        return ret;
    }

    void World::Init()
    {
        m_Running = true;

        ProcessPendingComponents();
        ProcessPendingSystems();
        ProcessPendingComponents(); // system init may have modified components

        m_Running = false;
    }

    void World::Update()
    {
        PROFILE_SCOPE("World::Update")
        ProcessAllPending();
        m_Running = true;

        RunOnAllEngineSystems([this](const Id& systemId)
        {
            PROFILE_BEGIN_THREAD()
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Update();
            }
        }, true, true);

        RunOnAllAppSystems([this](auto&& systemId)
        {
            PROFILE_BEGIN_THREAD()
            if (auto* system = m_AppSystems[systemId].instance)
            {
                system->Update();
            }
        }, true, true);

        m_Running = false;

        for (auto* signal: m_PendingSignals)
        {
            signal->Execute();
        }
        m_PendingSignals.clear();

#if SPARK_EDITOR
        if (m_EntitiesChangedThisFrame)
        {
            Application::Get()->GetEditorRuntime()->OnEntitiesChanged();
            ProcessAllPending();
            m_EntitiesChangedThisFrame = false;
        }
#endif
    }

    void World::Render()
    {
        m_Running = true;

        RunOnAllEngineSystems([this](auto&& systemId)
        {
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Render();
            }
        }, false, false);

        RunOnAllAppSystems([this](auto&& systemId)
        {
            if (auto* system = m_AppSystems[systemId].instance)
            {
                system->Render();
            }
        }, false, false);

        m_Running = false;
    }

    void World::Shutdown()
    {
        m_Running = true;

        RunOnAllEngineSystems([this](auto&& systemId)
        {
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, true, true);

        RunOnAllAppSystems([this](auto&& systemId)
        {
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, true, true);

        m_Running = false;

        ProcessAllPending();
    }

    void World::RebuildSystemUpdateGroups(std::vector<std::vector<Id>>& updateGroups,
                                          std::unordered_map<Id, SystemRecord>& systems)
    {
        updateGroups.clear();

        std::vector<std::pair<Id, SystemRecord>> sortedSystemRecords = {};
        std::ranges::for_each(systems, [&sortedSystemRecords](const std::pair<Id, SystemRecord>& kvp)
        {
            sortedSystemRecords.push_back(kvp);
        });
        std::ranges::sort(sortedSystemRecords,
                          [](const std::pair<Id, SystemRecord>& a, const std::pair<Id, SystemRecord>& b)
                          {
                              bool aDependsOnB = a.second.instance->DependsOn(b.first);
                              bool bDependsOnA = b.second.instance->DependsOn(a.first);
                              SPARK_ASSERT((!aDependsOnB && !bDependsOnA) || aDependsOnB != bDependsOnA);
                              if (aDependsOnB || bDependsOnA)
                              {
                                  return !aDependsOnB || bDependsOnA;
                              }
                              else
                              {
                                  return a.first < b.first;
                              }
                          });

        std::unordered_map<Id, std::map<Id, ComponentMutability::Type>> usedComponents;
        std::unordered_map<Id, size_t> updateGroupLookup;
        for (const auto& [id, systemRecord]: systems)
        {
            if (systemRecord.instance)
            {
                auto& map = (*usedComponents.insert(std::make_pair(id, std::map<Id, ComponentMutability::Type>())).first).second;
                // TODO not taking variant components into account here
                for (const auto& usedComp : systemRecord.instance->GetDeclaration().componentUsage)
                {
                    map[usedComp.id] = usedComp.mutability;
                }
                for (const auto& usedComp : systemRecord.instance->GetDeclaration().singletonComponentUsage)
                {
                    map[usedComp.id] = usedComp.mutability;
                }
            }
        }

        for (const auto& [id, systemRecord]: sortedSystemRecords)
        {
            if (systemRecord.instance)
            {
                bool systemAdded = false;
                for (size_t i = 0; i < updateGroups.size(); ++i)
                {
                    auto& updateGroup = updateGroups[i];

                    bool blockedOnDependency = false;
                    for (Id dependency: systemRecord.instance->GetDeclaration().dependencies)
                    {
                        if (SPARK_VERIFY(updateGroupLookup.contains(dependency), "Dependency has not been assigned an update group!")
                            && updateGroupLookup[dependency] >= i)
                        {
                            blockedOnDependency = true;
                            break;
                        }
                    }

                    if (blockedOnDependency)
                    {
                        continue;
                    }

                    bool conflict = false;
                    for (Id other_id: updateGroup)
                    {
                        const auto& usedComps = usedComponents[id];
                        conflict = std::ranges::any_of(usedComponents[other_id],
                                                       [usedComps](
                                                   const std::pair<Id, ComponentMutability::Type>& compId)
                                                       {
                                                           return usedComps.contains(compId.first)
                                                                  && (compId.second == ComponentMutability::Mutable ||
                                                                      usedComps.at(compId.first) ==
                                                                      ComponentMutability::Mutable);
                                                       });

                        if (conflict)
                        {
                            break;
                        }
                    }

                    if (!conflict)
                    {
                        systemAdded = true;
                        updateGroup.push_back(id);
                        updateGroupLookup[id] = i;
                        break;
                    }
                }

                if (!systemAdded)
                {
                    updateGroups.push_back({id});
                    updateGroupLookup[id] = updateGroups.size() - 1;
                }
            }
        }
    }

    bool World::IsChildOf(Id entity, Id parent)
    {
        return HasComponent(entity, CreateChildRelationship(parent).GetId());
    }

    void World::DestroyObserver(Id id)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ObserverMutex);
        for (auto& [comp, kvp]: m_Observers)
        {
            if (kvp.contains(id))
            {
                kvp.erase(id);
                break;
            }
        }
    }

    const String* World::GetName(uint64_t id)
    {
        if (id == s_InvalidEntity || !m_IdMetaMap.contains(id))
        {
            return nullptr;
        }

        return &m_IdMetaMap.at(id).name;
    }

    int32_t* World::GetFlags(uint64_t id)
    {
        if (id == s_InvalidEntity || !m_IdMetaMap.contains(id))
        {
            return nullptr;
        }

        return &m_IdMetaMap.at(id).flags;
    }

    void World::CollectArchetypes(const std::vector<ComponentUsage>& components,
                           std::set<Archetype*>& archetypes)
    {
        for (const auto& compId: components)
        {
            if (m_ComponentRecords.contains(compId.id))
            {
                auto& compRecord = m_ComponentRecords.at(compId.id);
                for (const auto& archetypeId: compRecord.archetypeRecords | std::views::keys)
                {
                    auto& archetype = m_Archetypes.at(archetypeId);
                    if (archetypes.contains(&archetype))
                    {
                        continue;
                    }

                    bool containsAll = true;
                    for (auto comp : components)
                    {
                        containsAll &= std::ranges::contains(archetype.type, comp.id);
                    }

                    if (!containsAll)
                    {
                        continue;
                    }

                    archetypes.insert(&archetype);
                }
            }
        }
    }

    void World::CollectArchetypesWithVariant(std::vector<ComponentUsage> components,
                                      const ComponentUsage& variantUsage,
                                      std::set<VariantQueryArchetype>& archetypes)
    {
        if (variantUsage.id != s_InvalidEntity)
        {
            components.push_back(variantUsage);
        }

        for (const auto& compId: components)
        {
            if (m_ComponentRecords.contains(compId.id))
            {
                auto& compRecord = m_ComponentRecords.at(compId.id);
                for (const auto& archetypeId: compRecord.archetypeRecords | std::views::keys)
                {
                    auto& archetype = m_Archetypes.at(archetypeId);
                    auto it = std::find_if(archetypes.begin(), archetypes.end(), [&archetype](const VariantQueryArchetype& other){ return other.archetype == &archetype; });
                    if (it != archetypes.end())
                    {
                        continue;
                    }

                    bool containsAll = true;
                    for (auto comp : components)
                    {
                        containsAll &= std::ranges::contains(archetype.type, comp.id);
                    }

                    if (!containsAll)
                    {
                        continue;
                    }

                    archetypes.insert(VariantQueryArchetype(&archetype, variantUsage));
                }
            }
        }
    }

    bool World::ValidateChildQuery(const System* system,
                                   const ChildQueryDeclaration& declaration)
    {
        const auto& systemDec = system->GetDeclaration();
        for (const auto& comp: declaration.componentUsage)
        {
            auto compIt = std::ranges::find_if(systemDec.componentUsage, [comp](const ComponentUsage& usage) { return usage.id == comp.id; });
            auto childIt = std::ranges::find_if(systemDec.childQuerys, [comp](const ComponentUsage& usage) { return usage.id == comp.id; });
            if (!SPARK_VERIFY((compIt != systemDec.componentUsage.end() && comp.mutability <= compIt->mutability) ||
                                ((childIt != systemDec.childQuerys.end() && comp.mutability <= childIt->mutability))))
            {
                return false;
            }
        }

        return true;
    }

    Relationship World::CreateChildRelationship(Id entity)
    {
        Relationship childOf;
        childOf.SetId(bits::Pack64(bits::UnpackA64(components::ChildOf::GetComponentId()), bits::UnpackA64(entity)));
        return childOf;
    }

    void World::RegisterRelationship(uint64_t id)
    {
        if (!m_ComponentRecords.contains(id))
        {
            m_ComponentRecords.insert(std::make_pair(id,
                                                     ComponentRecord
                                                     {
                                                         .type = reflect::ClassResolver<Relationship>::get(),
                                                         .archetypeRecords = {}
                                                     }));
            Id leftId = bits::Pack64(bits::UnpackA64(id), 0);
            Id rightId = bits::Pack64(bits::UnpackB64(id), 0);
            m_IdMetaMap[id] = {
                std::format("{0} => {1}", leftId.name->Data(), rightId.id),
                0
            };
        }
    }

    void World::RunOnAllSystems(const std::function<void(const Id&)>& func,
                                const std::vector<std::vector<Id>>& systemUpdateGroups, bool parallel, bool processPending)
    {
        PROFILE_SCOPE("World::RunOnAllSystems")
        for (const auto& updateGroup: systemUpdateGroups)
        {
            m_UpdateMode = parallel && updateGroup.size() > 1 ? UpdateMode::MultiThreaded : UpdateMode::SingleThreaded;
            if (m_UpdateMode == UpdateMode::MultiThreaded)
            {
                threads::ParallelForEach(updateGroup, func);
            }
            else
            {
                std::for_each(updateGroup.begin(),
                              updateGroup.end(),
                              func);
            }

            if (processPending)
            {
                ProcessAllPending();
            }
        }
    }

    void World::RunOnAllAppSystems(const std::function<void(const Id&)>& func, bool parallel, bool processPending)
    {
        RunOnAllSystems(func, m_AppSystemUpdateGroups, parallel, processPending);
    }

    void World::RunOnAllEngineSystems(const std::function<void(const Id&)>& func, bool parallel, bool processPending)
    {
        RunOnAllSystems(func, m_EngineSystemUpdateGroups, parallel, processPending);
    }

    void World::DestroyEntity(Id entity)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_EntityMutex);
        m_PendingEntityDeletions.push_back(entity);
    }

    std::vector<Id> World::GetEntities() const
    {
        return util::ToKeyArray<Id>(m_EntityRecords);
    }

    std::vector<Id> World::GetRootEntities()
    {
        std::vector<Id> ret;
        Each({ ComponentUsage(components::RootComponent::GetComponentId(), ComponentMutability::Immutable) },
             {},
            [&ret](const SystemUpdateData& updateData)
            {
                const auto& entities = updateData.GetEntities();
                ret.reserve(ret.size() + entities.size());
                ret.insert(ret.end(), entities.begin(), entities.end());
            }, false);

        return ret;
    }

    void World::AddRelationship(Id entity, const Relationship& relationship)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterRelationship(relationship.GetId());

        if (!SPARK_VERIFY(!HasComponent(entity, relationship.GetId())))
        {
            return;
        }

        m_PendingComponentCreations.emplace_back(PendingComponent{
            .entity = entity, .comp = relationship.GetId(), .tempData = m_TempStore.Alloc<Relationship>(relationship)
        });
        SPARK_ASSERT(m_PendingComponentCreations.back().comp.name != nullptr);
    }

    void World::RemoveRelationship(Id entity, const Relationship& relationship)
    {
        auto guard = MaybeLockGuard(m_UpdateMode, &m_ComponentMutex);
        RegisterRelationship(relationship.GetId());

        if (!SPARK_VERIFY(HasComponent(entity, relationship.GetId())))
        {
            return;
        }

        m_PendingComponentDeletions.emplace_back(std::pair<Id, Id>{entity, relationship.GetId()});
    }

    void World::AddChild(Id entity, Id childEntity)
    {
        AddRelationship(childEntity, CreateChildRelationship(entity));
        if (!HasComponent<components::ParentComponent>(entity))
        {
            AddComponent<components::ParentComponent>(entity);
        }

        auto& parentRecord =  m_EntityRecords.at(entity);
        parentRecord.children.push_back(childEntity);
        m_EntityRecords.at(childEntity).parent = entity;

        if (!HasComponent<components::RootComponent>(entity) && !HasRelationshipWildcard<components::ChildOf>(entity))
        {
            AddComponent<components::RootComponent>(entity);
        }

        if (HasComponent<components::RootComponent>(childEntity))
        {
            RemoveComponent<components::RootComponent>(childEntity);
        }
    }

    void World::RemoveChild(Id entity, Id childEntity)
    {
        RemoveRelationship(childEntity, CreateChildRelationship(entity));

        auto& parentRecord = m_EntityRecords.at(entity);
        auto [first, last] = std::ranges::remove(parentRecord.children, childEntity);
        parentRecord.children.erase(first, last);

        if (HasComponent<components::ParentComponent>(entity) && parentRecord.children.empty())
        {
            RemoveComponent<components::ParentComponent>(entity);
        }

        m_EntityRecords.at(childEntity).parent = s_InvalidEntity;
    }

    void World::ProcessPendingComponents()
    {
        PROFILE_SCOPE("World::ProcessPendingComponents")
        auto creationsCopy = m_PendingComponentCreations;
        m_PendingComponentCreations.clear();
        for (const auto& pendingComp: creationsCopy)
        {
            AddComponentInternal(pendingComp);
        }

        for (const auto& pendingComp: creationsCopy)
        {
            if (m_Observers.contains(pendingComp.comp))
            {
                for (const auto& observer: m_Observers.at(pendingComp.comp))
                {
                    observer.second->Added(pendingComp.entity, GetComponent(pendingComp.entity, pendingComp.comp));
                }
            }
        }

        auto deletionsCopy = m_PendingComponentDeletions;
        m_PendingComponentDeletions.clear();

        for (const auto& [entity, component]: deletionsCopy)
        {
            RemoveComponentInternal(entity, component);
        }
    }

    void World::ProcessPendingSystems()
    {
        PROFILE_SCOPE("World::ProcessPendingSystems")
        ProcessPendingEngineSystems();
        ProcessPendingAppSystems();
    }

    void World::ProcessPendingSystems(std::vector<std::pair<Id, SystemDeclaration>>& pendingCreations,
                                      std::vector<Id>& pendingDeletions,
                                      std::unordered_map<Id, SystemRecord>& systemRecords,
                                      std::vector<std::vector<Id>>& systsemUpdateGroups,
                                      std::vector<Id>& freeSystems)
    {
        bool shouldRebuildUpdateGroups = !pendingCreations.empty() || !pendingDeletions.empty(); {
            auto safeCopy = pendingCreations;
            pendingCreations.clear();
            for (const auto& [systemId, relationships]: safeCopy)
            {
                CreateSystemInternal(systemRecords, systemId, relationships);
                systemRecords.at(systemId).instance->Init();
            }
        } {
            auto safeCopy = pendingDeletions;
            pendingDeletions.clear();
            for (Id system: safeCopy)
            {
                systemRecords.at(system).instance->Shutdown();
                DestroySystemInternal(systemRecords, freeSystems, system);
            }
        }

        if (shouldRebuildUpdateGroups)
        {
            RebuildSystemUpdateGroups(systsemUpdateGroups, systemRecords);
        }
    }

    void World::ProcessPendingAppSystems()
    {
        ProcessPendingSystems(m_PendingAppSystemCreations, m_PendingAppSystemDeletions, m_AppSystems,
                              m_AppSystemUpdateGroups, m_FreeSystems);
    }

    void World::ProcessPendingEngineSystems()
    {
        ProcessPendingSystems(m_PendingEngineSystemCreations, m_PendingEngineSystemDeletions, m_EngineSystems,
                              m_EngineSystemUpdateGroups, m_FreeSystems);
    }

    void World::CreateSystemInternal(std::unordered_map<Id, SystemRecord>& systemMap,
                                     Id system,
                                     const SystemDeclaration& pendingSystem)
    {
        if (SPARK_VERIFY(systemMap.contains(system) && systemMap.at(system).instance == nullptr))
        {
            auto& record = systemMap.at(system);
            record.instance = static_cast<System*>(record.type->heap_constructor());
            record.instance->m_Declaration = pendingSystem;
        }
    }

    void World::DestroySystemInternal(std::unordered_map<Id, SystemRecord>& systemMap,
                                      std::vector<Id>& freeSystems,
                                      Id system)
    {
        if (SPARK_VERIFY(systemMap.contains(system)))
        {
            auto& systemRecord = systemMap.at(system);
            delete systemRecord.instance;
            systemMap.erase(system);
            freeSystems.push_back(system);
        }
    }

    Id World::NewObserver()
    {
        return m_ObserverCounter++;
    }

    Id World::RecycleObserver()
    {
        const auto id = m_FreeObservers.back();
        m_FreeObservers.pop_back();

        return id;
    }

    void World::AddComponentInternal(const World::PendingComponent& pendingComp)
    {
        if (!SPARK_VERIFY(!HasComponent(pendingComp.entity, pendingComp.comp)))
        {
            return;
        }

        EntityRecord& record = m_EntityRecords.at(pendingComp.entity);
        Archetype* archetype = record.archetype;
        Archetype* next_archetype = GetNextArchetype(archetype, pendingComp.comp, true, true);
        record.entity_idx = MoveEntity(pendingComp.entity, archetype, record.entity_idx, next_archetype);
        record.archetype = next_archetype;
        SPARK_ASSERT(record.archetype);
        void* compData = GetComponent(pendingComp.entity, pendingComp.comp);
        m_ComponentRecords[pendingComp.comp].type->inplace_copy_constructor(compData, pendingComp.tempData);
    }

    void World::RemoveComponentInternal(Id entity, Id comp)
    {
        if (!SPARK_VERIFY(HasComponent(entity, comp)))
        {
            return;
        }

        void* data = GetComponent(entity, comp);
        if (m_Observers.contains(comp))
        {
            for (const auto& observer: m_Observers.at(comp))
            {
                observer.second->Removed(entity, data);
            }
        }

        m_ComponentRecords[comp].type->destructor(data);
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        Archetype* next_archetype = GetNextArchetype(archetype, comp, false, true);
        record.entity_idx = MoveEntity(entity, archetype, record.entity_idx, next_archetype);
        record.archetype = next_archetype;
        SPARK_ASSERT(record.archetype);
    }

    uint64_t World::NewSystem()
    {
        return m_SystemCounter++;
    }

    uint64_t World::RecycleSystem()
    {
        const auto id = m_FreeSystems.back();
        m_FreeSystems.pop_back();

        return id;
    }

    void World::ProcessPendingEntityDeletions()
    {
        PROFILE_SCOPE("World::ProcessPendingEntityDeletions")
        auto safeCopy = m_PendingEntityDeletions;
        m_PendingEntityDeletions.clear();
        for (Id entity: safeCopy)
        {
            DestroyEntityInternal(entity);
        }
    }

    void World::AddPendingSignal(BaseSignal* signal)
    {
        m_PendingSignals.push_back(signal);
    }

    void World::OnSignalDestroyed(BaseSignal* signal)
    {
        if (!m_ClearingTempObjects)
        {
            const auto [first, last] = std::ranges::remove(m_PendingSignals, signal);
            m_PendingSignals.erase(first, last);
        }
    }

    const std::vector<Id>& World::GetChildren(Id entity) const
    {
        return m_EntityRecords.at(entity).children;
    }

    Id World::GetParent(Id entity) const
    {
        return m_EntityRecords.at(entity).parent;
    }
}
