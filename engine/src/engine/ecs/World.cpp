#include "World.h"


#include <Widgets.generated.h>
#include <easy/profiler.h>

#include "engine/render/render_fwd.h"
#include "SceneSaveData.h"
#include "components/ParentComponent.h"
#include "components/RootComponent.h"
#include "engine/bits/FlagUtil.h"
#include "engine/containers/MapUtil.h"
#include "engine/ecs/Signal.h"
#include "engine/ecs/System.h"
#include "engine/io/VFS.h"
#include "engine/reflect/Reflect.h"
#include "engine/reflect/Util.h"
#include "engine/render/Renderer.h"
#include "engine/render/singleton_components/MeshRenderComponent.h"
#include "engine/string/util/StringUtil.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "SystemUtil.h"
#include "engine/render/opengl/DeferredOpenGLOperations.h"

namespace se::ecs
{
    thread_local int World::EachDepth = 0;

    Id World::CreateEntity(const std::string& name)
    {
        return CreateEntity(m_DefaultScene, name);
    }

    Id World::CreateEntity(const Id& scene,
        const std::string& name)
    {
        auto guard = std::lock_guard(m_EntityMutex);
        uint64_t entityId;
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

        int32_t flags = 0; // TODO remove?
        m_IdMetaMap[entityId] = { name, flags, scene };
        auto emptyArchetype = GetArchetype({}, true);
        SPARK_ASSERT(!m_EntityRecords.contains(entityId));
        [[maybe_unused]] auto result = m_EntityRecords.insert(std::make_pair(entityId, EntityRecord
                                              {
                                                  .archetype = emptyArchetype,
                                                  .entity_idx = 0 // not valid for null archetype
                                              }));
        m_SceneRecords.at(scene).entities.push_back(entityId);

        SPARK_ASSERT(result.second);

        AddComponent<components::RootComponent>(entityId);
        [[maybe_unused]] auto& record = m_EntityRecords.at(entityId);
        SPARK_ASSERT(record.archetype);

#if SPARK_EDITOR
        auto editorRuntime = Application::Get()->GetEditorRuntime();
        if (scene != editorRuntime->GetEditorScene().id)
        {
            m_EntitiesChangedThisFrame = true;
        }
#endif

        return entityId;
    }

    void World::RenameEntity(const Id& entity,
        const std::string& name)
    {
        auto guard = std::lock_guard(m_EntityMutex);
        auto it = m_IdMetaMap.find(entity);
        if (SPARK_VERIFY(it != m_IdMetaMap.end()))
        {
            it->second.name = name;
        }
    }

    void World::DestroyEngineSystem(const Id& id)
    {
        auto guard = std::lock_guard(m_SystemMutex);
        m_PendingEngineSystemDeletions.push_back(id);
    }

    System* World::GetAppSystem(const Id& systemId)
    {
        const auto it = m_AppSystems.find(systemId);
        if (it != m_AppSystems.end())
        {
            return it->second.instance;
        }

        return nullptr;
    }

    std::vector<reflect::ObjectBase*> World::GetSingletonComponents() const
    {
        return ::se::util::ToValueArray<reflect::ObjectBase*>(m_SingletonComponents);
    }

    void World::DestroyAppSystem(const Id& id)
    {
        auto guard = std::lock_guard(m_SystemMutex);
        m_PendingAppSystemDeletions.push_back(id);
    }

    uint64_t World::NewEntity()
    {
        return m_EntityCounter++;
    };

    uint64_t World::RecycleEntity()
    {
        const auto entity = m_FreeEntities.back();
        m_FreeEntities.pop_back();

        return entity;
    }

    void* World::GetComponent(const Id& entity, const Id& component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        auto& compInfo = m_ComponentRecords.at(component);
        Archetype* archetype = record.archetype;
        auto it = compInfo.archetypeRecords.find(archetype->id);
        bool hasComp = it != compInfo.archetypeRecords.end();

        if (!hasComp)
        {
            // check pending comps
            for (const auto& pendingComp: m_PendingComponentCreations)
            {
                if (pendingComp.entity == entity && pendingComp.comp == component)
                {
                    return pendingComp.tempData;
                }
            }

            return nullptr;
        }

        ArchetypeComponentKey& a_record = it->second;
        const auto& compList = archetype->components.at(a_record);
        void* comp = compList.GetComponent(record.entity_idx);
        return comp;
    }

    void World::DestroyEntityInternal(const Id& entity)
    {
        if (entity == s_InvalidEntity)
        {
            return;
        }

        const auto it = m_EntityRecords.find(entity);
        if (!SPARK_VERIFY(it != m_EntityRecords.end()))
        {
            return;
        }

#if SPARK_EDITOR
        auto editorRuntime = Application::Get()->GetEditorRuntime();
        if (entity == editorRuntime->GetSelectedEntity())
        {
            editorRuntime->SelectEntity(s_InvalidEntity);
        }
        const auto& scene = m_IdMetaMap[entity].scene;
        if (scene != editorRuntime->GetEditorScene())
        {
            m_EntitiesChangedThisFrame = true;
        }
#endif

        auto& record = it->second;

        auto safeCopy = record.children;
        for (const auto& child: safeCopy)
        {
            DestroyEntityInternal(child);
        }

        safeCopy = record.archetype->typeVector;
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

        auto& sceneRecord = m_SceneRecords.at(*entity.scene);
        auto [first, last] = std::ranges::remove(sceneRecord.entities, entity);
        sceneRecord.entities.erase(first, last);
        std::erase(record.archetype->entities, entity);
        m_EntityRecords.erase(entity);
        m_FreeEntities.push_back(entity);
    }

    bool World::HasComponent(const Id& entity, const Id& component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        if (!archetype)
        {
            return false;
        }

        const auto& compInfo = m_ComponentRecords[component];
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
        TypeVector typeVector = {};
        for (size_t i = 0; i < NumComponents; ++i)
        {
            if (type.test(i))
            {
                typeVector.push_back(i + 1);
            }
        }

        Archetype archetype = {
            .id = m_ArchetypeCounter++,
            .type = type,
            .typeVector = typeVector,
            .entities = {},
            .components = {},
            .edges = {}
        };

        archetype.components.reserve(typeVector.size());
        for (const auto& compType : typeVector)
        {
            SPARK_ASSERT(compType.name != nullptr);
            auto& compInfo = m_ComponentRecords[compType];
            archetype.components.emplace_back(compInfo.type->size);
        }

        m_Archetypes.insert(std::make_pair(archetype.id, archetype));
        m_ArchetypeTypeLookup.insert(std::make_pair(type, archetype.id));

        for (size_t i = 0; i < typeVector.size(); ++i)
        {
            auto& compInfo = m_ComponentRecords[typeVector[i]];
            compInfo.archetypeRecords.insert(std::make_pair(archetype.id, i));
        }
    }

    size_t World::MoveEntity(const Id& entity, Archetype* archetype, size_t entityIdx, Archetype* nextArchetype)
    {
        //need to update all entity records when an archetype is updated ew.
        std::vector<std::pair<Id, uint8_t*>> compData;
        for (size_t i = 0; i < archetype->components.size(); ++i)
        {
            Id id = archetype->typeVector[i];
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
            if (!componentsAccountedFor.contains(nextArchetype->typeVector[i]))
            {
                auto compInfo = m_ComponentRecords[nextArchetype->typeVector[i]];
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

        if (archetype->entities.empty() && !archetype->typeVector.empty())
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

            for (size_t i = 0; i < archetype->typeVector.size(); ++i)
            {
                auto& compInfo = m_ComponentRecords[archetype->typeVector[i]];
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
                if (!addType.test(component - 1))
                {
                    addType.set(component - 1);
                }
                edge.add = GetArchetype(addType, createIfMissing);
            }
            else
            {
                auto removeType = archetype->type;
                removeType.set(component - 1, false);
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
        EASY_FUNCTION();
        ProcessAllPending();
        m_Running = true;

#if OPENGL_RENDERER
        auto deferredOps = render::opengl::DeferredOpenGLOperations::Get();
#endif

        RunOnAllSystems([this](const Id& systemId)
        {
            EASY_BLOCK(systemId.name->c_str());
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Update();
            }
        }, m_EngineSystemUpdateGroups, true, true);

#if OPENGL_RENDERER
       deferredOps->ExecuteDeferredOps();
#endif

#if SPARK_EDITOR
        auto editor = Application::Get()->GetEditorRuntime();
        if (editor->InGameMode())
#endif
        {
            if (!m_Paused)
            {
                RunOnAllSystems([this](auto&& systemId)
                {
                    EASY_BLOCK(systemId.name->c_str());
                    if (auto* system = m_AppSystems[systemId].instance)
                    {
                        system->Update();
                    }
                }, m_AppSystemUpdateGroups, true, true);
            }
        }

#if OPENGL_RENDERER
        deferredOps->ExecuteDeferredOps();
#endif

        m_Running = false;

        auto safeCopy = m_PendingSignals;
        m_PendingSignals.clear();
        for (auto* signal: safeCopy)
        {
            EASY_BLOCK("Execute Signals");
            signal->Execute();
        }
        ProcessAllPending();

#if SPARK_EDITOR
        if (m_EntitiesChangedThisFrame)
        {
            Application::Get()->GetEditorRuntime()->OnEntitiesChanged();
            ProcessAllPending();
            m_EntitiesChangedThisFrame = false;
        }
#endif

        //DumpWidgetHeirachy();
    }

    void World::Render()
    {
        GetSingletonComponent<render::singleton_components::MeshRenderComponent>()->layerRenderGroups.clear();

        m_Running = true;

        auto renderer = render::Renderer::Get<render::Renderer>();
        bool parallel = renderer->SupportsMultiThreadedRendering();

        RunOnAllSystems([this](auto&& systemId)
        {
            EASY_BLOCK(systemId.name->c_str());
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Render();
            }
        }, m_EngineSystemRenderGroups, parallel, false);

        RunOnAllSystems([this](auto&& systemId)
        {
            EASY_BLOCK(systemId.name->c_str());
            if (auto* system = m_AppSystems[systemId].instance)
            {
                system->Render();
            }
        }, m_AppSystemRenderGroups, parallel, false);

        m_Running = false;
    }

    void World::Shutdown()
    {
        m_Running = true;

        RunOnAllSystems([this](auto&& systemId)
        {
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, m_EngineSystemUpdateGroups, true, true);

        RunOnAllSystems([this](auto&& systemId)
        {
            if (auto* system = m_AppSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, m_AppSystemUpdateGroups, true, true);

        RunOnAllSystems([this](auto&& systemId)
        {
            if (auto* system = m_EngineSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, m_EngineSystemRenderGroups, true, true);

        RunOnAllSystems([this](auto&& systemId)
        {
            if (auto* system = m_AppSystems[systemId].instance)
            {
                system->Shutdown();
            }
        }, m_AppSystemRenderGroups, true, true);

        m_Running = false;

        ProcessAllPending();
    }

    void World::TogglePause()
    {
        m_Paused = !m_Paused;
    }

    Id World::LoadScene(std::string path)
    {
        auto db = asset::binary::Database::Load(path, true);
        auto type = reflect::TypeResolver<SceneSaveData>::get();
        SceneSaveData obj = {};
        auto root = db->GetRoot();
        type->Deserialize(&obj, root, {});

        // TODO Save scene name
        auto scene = CreateScene("Scene");

        std::unordered_map<uint64_t, Id> idRemap = {};
        for (const auto& entity : obj.m_Entities)
        {
            auto newId = CreateEntity(scene, entity.name);
            idRemap[entity.entity] = newId;
            auto& meta = m_IdMetaMap[newId];
            meta.flags = entity.flags;

            for (auto* component : entity.components)
            {
                if (component->GetStaticComponentId() != components::RootComponent::GetComponentId()) // Already added by CreateEntity.
                {
                    auto* reflect = component->GetReflectType();
                    void* tempData = m_TempStore.AllocUninitialized<Component>(reflect->size);
                    reflect->inplace_copy_constructor(tempData, component);
                    m_PendingComponentCreations.emplace_back(PendingComponent { .entity = newId, .comp = component->GetStaticComponentId(), .tempData = tempData });
                }

                delete component;
            }
        }

        for (const auto& entity : obj.m_Entities)
        {
            const auto& remappedEntity = idRemap[entity.entity];
            for (auto child : entity.children)
            {
                AddChild(remappedEntity, idRemap[child]);
            }
        }

        constexpr const char* editor_scene = "editor_scene_";
        auto it = path.find(editor_scene);
        if (it != std::string::npos)
        {
            path.replace(it, string::util::ConstLength(editor_scene), "");
        }
        m_SceneRecords[scene].path = path;

#if SPARK_EDITOR
        m_EntitiesChangedThisFrame = true;
#endif

        return scene;
    }

    void World::UnloadScene(const Id& scene)
    {
        auto it = m_SceneRecords.find(scene);
        if (!SPARK_VERIFY(it != m_SceneRecords.end()))
        {
            return;
        }

        for (const Id& entity : it->second.entities)
        {
            if (HasComponent<components::RootComponent>(entity))
            {
                DestroyEntity(entity);
            }
        }
    }

#if SPARK_EDITOR
    void World::SaveSceneToTemp(const Id& id)
    {
        auto it = m_SceneRecords.find(id);
        if (it != m_SceneRecords.end())
        {
            const auto& record = it->second;
            auto lastSlashIndex = record.path.find_last_of("/");
            auto fileName = record.path.substr(lastSlashIndex + 1);
            SaveScene(it->first, std::format("/tmp/editor_scene_{}", fileName), true);
        }
    }

    Id World::ReloadSceneFromTemp(const Id& id)
    {
        std::string fileName = {};
        auto it = m_SceneRecords.find(id);
        if (it != m_SceneRecords.end())
        {
            auto lastSlashIndex = it->second.path.find_last_of("/");
            fileName = it->second.path.substr(lastSlashIndex + 1);
            UnloadScene(it->first);
        }

        ProcessAllPending();
        m_SceneRecords.erase(it->first);
        return LoadScene(std::format("/tmp/editor_scene_{}", fileName));
    }

    std::vector<std::pair<Id, reflect::Class*>> World::GetAllComponentTypes()
    {
        std::vector<std::pair<Id, reflect::Class*>> ret ={};
        ret.reserve(m_ComponentRecords.size());
        for (const auto& [id, comp] : m_ComponentRecords)
        {
            if (id != s_InvalidEntity && comp.type != nullptr)
            {
                ret.push_back(std::make_pair(id, comp.type));
            }
        }

        std::ranges::sort(ret, [](const std::pair<Id, reflect::Class*>& a, const std::pair<Id, reflect::Class*>& b)
        {
            return a.second->name < b.second->name;
        });

        return ret;
    }
#endif

    void World::SaveScene(const Id& scene, const std::string& path, bool binary)
    {
        auto it = m_SceneRecords.find(scene);
        if (!SPARK_VERIFY(it != m_SceneRecords.end()))
        {
            return;
        }

        auto db = asset::binary::Database::Create(false);
        auto type = reflect::TypeResolver<SceneSaveData>::get();
        db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
        SceneSaveData saveData = {};

        for (const Id& entity : it->second.entities)
        {
            SceneEntityData entityData = {};
            entityData.entity = entity;
            entityData.name = *entity.name;
            entityData.flags = *entity.flags;
            for (const auto& child : GetChildren(entity))
            {
                entityData.children.push_back(child.id);
            }

            EntityRecord& record = m_EntityRecords.at(entity);
            Archetype* archetype = record.archetype;
            for (const Id& compType : archetype->typeVector)
            {
                auto* compData = GetComponent(entity, compType);
                entityData.components.push_back(static_cast<Component*>(compData));
            }

            saveData.m_Entities.push_back(entityData);
        }

        auto root = db->GetRoot();
        type->Serialize(&saveData, root, {});

        if (binary)
        {
            db->Save(path);
        }
        else
        {
            auto json = db->ToJson();
            io::VFS::Get().WriteText(path, json.dump(4));
        }
    }

    void RecurseWidgetChildren(World* world, const Id& entity, nlohmann::ordered_json& parentJson)
    {
        for (const auto& child: world->GetChildren(entity))
        {
            if (world->HasComponent<ui::components::RectTransformComponent>(child))
            {
                auto rectTransform = world->GetComponent<ui::components::RectTransformComponent>(child);
                auto db = reflect::SerialiseType<ui::components::RectTransformComponent>(rectTransform);
                nlohmann::ordered_json json;
                auto dbJson = db->ToJson();
                json["id"] = child.id;
                json["name"] = child.name->data();
                json["rect"] = dbJson["root"];
                RecurseWidgetChildren(world, child, json);
                parentJson["children"].push_back(json);
            }
        }
    }

    void World::RebuildSystemUpdateGroups(std::vector<std::vector<Id>>& updateGroups,
                                          std::unordered_map<Id, SystemRecord>& systems,
                                          std::function<bool(System*)>&& predicate)
    {
        updateGroups.clear();

        std::unordered_map<Id, std::map<Id, ComponentMutability>> usedComponents;
        std::unordered_map<Id, size_t> updateGroupLookup;
        for (const auto& [id, systemRecord]: systems)
        {
            if (systemRecord.instance)
            {
                if (predicate(systemRecord.instance))
                {
                    auto& map = usedComponents.insert(std::make_pair(id, std::map<Id, ComponentMutability>())).first->second;
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
        }

        SystemSorter sorter = {};
        auto sortedSystems = sorter.Sort(systems);

        for (const auto& [id, record] : sortedSystems)
        {
            if (record.instance && predicate(record.instance))
            {
                bool systemAdded = false;
                for (size_t i = 0; i < updateGroups.size(); ++i)
                {
                    auto& updateGroup = updateGroups[i];

                    bool blockedOnDependency = false;
                    for (Id dependency: record.instance->GetDeclaration().dependencies)
                    {
                        if (SPARK_VERIFY(updateGroupLookup.contains(dependency) || !predicate(systems[dependency].instance), "Dependency has not been assigned an update group!")
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
                                                   const std::pair<Id, ComponentMutability>& compId)
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

    bool World::IsChildOf(const Id& entity, const Id& parent) const
    {
        return GetParent(entity) == parent;
    }

    void World::DestroyObserver(const Id& id)
    {
        auto guard = std::lock_guard(m_ObserverMutex);
        for (auto& kvp: m_Observers | std::views::values)
        {
            if (kvp.contains(id))
            {
                kvp.erase(id);
                break;
            }
        }
    }

    const std::string* World::GetName(uint64_t id) const
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

    const Id* World::GetScene(uint64_t id) const
    {
        if (id == s_InvalidEntity || !m_IdMetaMap.contains(id))
        {
            return nullptr;
        }

        return &m_IdMetaMap.at(id).scene;
    }

    void World::CollectArchetypes(const std::vector<ComponentUsage>& components,
                                    const VariantComponentUsage& variantComponent,
                                    std::vector<QueryArchetype>& archetypes)
    {
        Type type = {};
        for (const auto& comp: components)
        {
            type.set(comp.id - 1);
        }

        auto comps = variantComponent.components;
        if (comps.empty())
        {
            comps.emplace_back(NullComponentType::GetComponentId());
        }
        for (const auto& variantUsage : comps)
        {
            Type type_copy = type;
            if (variantUsage != s_InvalidEntity)
            {
                type_copy.set(variantUsage - 1);
            }

            for (const auto& compId: components)
            {
                if (m_ComponentRecords.contains(compId.id))
                {
                    auto& compRecord = m_ComponentRecords.at(compId.id);
                    for (const auto& archetypeId: compRecord.archetypeRecords | std::views::keys)
                    {
                        auto& archetype = m_Archetypes.at(archetypeId);

                        if (!(type_copy & ~archetype.type).none())
                        {
                            continue;
                        }

                        auto it = std::ranges::find_if(archetypes, [archetype](const QueryArchetype& queryType)
                        {
                            return queryType.archetype->id == archetype.id;
                        });

                        if (it != archetypes.end())
                        {
                            continue;
                        }

                        archetypes.emplace_back(QueryArchetype{ &archetype, variantUsage });
                    }
                }
            }
        }
    }

    bool World::ValidateHeirachyQuery(const System* system,
                                   const HeirachyQueryDeclaration& declaration)
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

    void World::RunOnAllSystems(const std::function<void(const Id&)>& func,
                                const std::vector<std::vector<Id>>& systemUpdateGroups, bool parallel, bool processPending)
    {
        EASY_BLOCK("World::RunOnAllSystems");
        for (const auto& updateGroup: systemUpdateGroups)
        {
            if (parallel && updateGroup.size() > 1)
            {
                threads::ParallelForEach(updateGroup, func);
            }
            else
            {
                std::ranges::for_each(updateGroup,
                                      func);
            }

            if (processPending)
            {
                ProcessAllPending();
            }
        }
    }

    void World::DestroyEntity(const Id& entity)
    {
        auto guard = std::lock_guard(m_EntityMutex);
        SPARK_ASSERT(!std::ranges::contains(m_PendingEntityDeletions, entity));
        m_PendingEntityDeletions.push_back(entity);
    }

    std::vector<Id> World::GetEntities() const
    {
        return ::se::util::ToKeyArray<Id>(m_EntityRecords);
    }

    std::vector<Id> World::GetRootEntities()
    {
        std::vector<Id> ret = {};
        RunQuery({ ComponentUsage(components::RootComponent::GetComponentId(), ComponentMutability::Immutable) },
            {},
             {},
            [&ret](const QueryResults& result)
            {
                ForEachArcheType(result, UpdateMode::SingleThreaded, false, [&ret](const SystemUpdateData& updateData)
                {
                    const auto& entities = updateData.GetEntities();
                    ret.reserve(ret.size() + entities.size());
                    ret.insert(ret.end(), entities.begin(), entities.end());
                });

            });

        return ret;
    }

#if SPARK_EDITOR
    void World::SetUIVisibility(bool visible)
    {
        SPARK_ASSERT(!m_Running);

        auto editor = Application::Get()->GetEditorRuntime();

        std::vector dec = {
            ComponentUsage(components::RootComponent::GetComponentId(), ComponentMutability::Immutable),
            ComponentUsage(ui::components::WidgetComponent::GetComponentId(), ComponentMutability::Mutable)
        };
        RunQuery(dec,
            {},
             {},
            [this, visible, editor](const QueryResults& result)
            {
                ForEachArcheType(result, UpdateMode::SingleThreaded, false, [this, visible, editor](const SystemUpdateData& updateData)
                {
                    const auto& entities = updateData.GetEntities();
                    auto* widgets = updateData.GetComponentArray<ui::components::WidgetComponent>();

                    for (size_t i = 0; i < entities.size(); ++i)
                    {
                        const auto& entity = entities[i];
                        if (*entity.scene != editor->GetEditorScene() &&
                            *entity.scene != m_DefaultScene)
                        {
                            auto& widget = widgets[i];
                            widget.dirty = true;
                            widget.parentVisibility = visible ? ui::Visibility::Visible : ui::Visibility::Collapsed;
                            widget.parentUpdateEnabled = visible;
                        }
                    }
                });
            });
    }
#endif

    void* World::AddComponent(const Id& entity,
        const Id& comp,
        reflect::Type* type)
    {
        auto guard = std::lock_guard(m_ComponentMutex);

        if (!SPARK_VERIFY(!HasComponent(entity, comp)))
        {
            return nullptr;
        }

        void* tempComp = m_TempStore.Alloc(type);
        m_PendingComponentCreations.emplace_back(PendingComponent { .entity = entity, .comp = comp, .tempData = tempComp });
        SPARK_ASSERT(m_PendingComponentCreations.back().comp.name != nullptr);
        return tempComp;
    }

    void World::AddChild(const Id& entity, const Id& childEntity)
    {
        if (!HasComponent<components::ParentComponent>(entity))
        {
            AddComponent<components::ParentComponent>(entity);
        }

        auto& parentRecord =  m_EntityRecords.at(entity);
        parentRecord.children.push_back(childEntity);
        m_EntityRecords.at(childEntity).parent = entity;

        if (!HasComponent<components::RootComponent>(entity) && GetParent(entity) == s_InvalidEntity)
        {
            AddComponent<components::RootComponent>(entity);
        }

        if (HasComponent<components::RootComponent>(childEntity))
        {
            RemoveComponent<components::RootComponent>(childEntity);
        }
    }

    void World::RemoveChild(const Id& entity, const Id& childEntity)
    {
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
        EASY_BLOCK("World::ProcessPendingComponents");
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
                for (const auto& val: m_Observers.at(pendingComp.comp) | std::views::values)
                {
                    val->Added(pendingComp.entity, GetComponent(pendingComp.entity, pendingComp.comp));
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
        EASY_BLOCK("World::ProcessPendingSystems");
        ProcessPendingEngineSystems();
        ProcessPendingAppSystems();
    }

    void World::ProcessPendingSystems(std::vector<std::pair<Id, SystemDeclaration>>& pendingCreations,
                                      std::vector<Id>& pendingDeletions,
                                      std::unordered_map<Id, SystemRecord>& systemRecords,
                                      std::vector<std::vector<Id>>& systemUpdateGroups,
                                      std::vector<std::vector<Id>>& systemRenderGroups)
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
                DestroySystemInternal(systemRecords, system);
            }
        }

        if (shouldRebuildUpdateGroups)
        {
            RebuildSystemUpdateGroups(systemUpdateGroups, systemRecords, [](System* system){ return system->ImplementsUpdateMethod(); });
            RebuildSystemUpdateGroups(systemRenderGroups, systemRecords, [](System* system){ return system->ImplementsRenderMethod(); });
        }
    }

    void World::ProcessPendingAppSystems()
    {
        ProcessPendingSystems(m_PendingAppSystemCreations, m_PendingAppSystemDeletions, m_AppSystems,
                              m_AppSystemUpdateGroups, m_AppSystemRenderGroups);
    }

    void World::ProcessPendingEngineSystems()
    {
        ProcessPendingSystems(m_PendingEngineSystemCreations, m_PendingEngineSystemDeletions, m_EngineSystems,
                              m_EngineSystemUpdateGroups, m_EngineSystemRenderGroups);
    }

    void World::CreateSystemInternal(std::unordered_map<Id, SystemRecord>& systemMap,
                                     const Id& system,
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
                                      const Id& system)
    {
        if (SPARK_VERIFY(systemMap.contains(system)))
        {
            auto& systemRecord = systemMap.at(system);
            delete systemRecord.instance;
            systemMap.erase(system);
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

    void World::AddComponentInternal(const PendingComponent& pendingComp)
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
        if (pendingComp.tempData)
        {
            m_ComponentRecords[pendingComp.comp].type->inplace_copy_constructor(compData, pendingComp.tempData);
        }
        else
        {
            m_ComponentRecords[pendingComp.comp].type->inplace_constructor(compData);
        }
    }

    void World::RemoveComponentInternal(const Id& entity, const Id& comp)
    {
        if (!SPARK_VERIFY(HasComponent(entity, comp)))
        {
            return;
        }

        void* data = GetComponent(entity, comp);
        if (m_Observers.contains(comp))
        {
            for (const auto& val: m_Observers.at(comp) | std::views::values)
            {
                val->Removed(entity, data);
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

    void World::ProcessPendingEntityDeletions()
    {
        EASY_FUNCTION();
        auto safeCopy = m_PendingEntityDeletions;
        m_PendingEntityDeletions.clear();
        for (const Id& entity: safeCopy)
        {
            DestroyEntityInternal(entity);
        }
    }

    void World::AddPendingSignal(BaseSignal* signal)
    {
        auto lock = std::lock_guard(m_SignalMutex);
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

    Id World::CreateScene(const std::string& name)
    {
        uint64_t sceneId;
        if (!m_FreeEntities.empty())
        {
            sceneId = RecycleEntity();
        }
        else
        {
            sceneId = NewEntity();
        }

        if (sceneId == s_InvalidEntity)
        {
            SPARK_ASSERT(false, "Max number of scenes reached.");
            return s_InvalidEntity;
        }

        if (m_DefaultScene == s_InvalidEntity)
        {
            m_DefaultScene = sceneId;
        }

        m_IdMetaMap[sceneId] = { name, 0, 0 };
        m_SceneRecords[sceneId] = { };

        return sceneId;
    }

    const std::vector<Id>& World::GetChildren(const Id& entity) const
    {
        auto it = m_EntityRecords.find(entity);
        if (it == m_EntityRecords.end())
        {
            static std::vector<Id> empty = {};
            return empty;
        }
        return it->second.children;
    }

    Id World::GetParent(const Id& entity) const
    {
        auto it = m_EntityRecords.find(entity);
        if (it == m_EntityRecords.end())
        {
            return s_InvalidEntity;
        }
        return it->second.parent;
    }

    void World::RemoveComponent(const Id& entity,
        const Id& comp)
    {
        auto guard = std::lock_guard(m_ComponentMutex);
        if (!SPARK_VERIFY(HasComponent(entity, comp)))
        {
            return;
        }
        m_PendingComponentDeletions.push_back(std::make_pair(entity, comp));
    }
}
