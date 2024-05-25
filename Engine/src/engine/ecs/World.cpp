#include "World.h"

#include "engine/reflect/Reflect.h"
#include "engine/ecs/System.h"

namespace se::ecs
{
    EntityId World::CreateEntity()
    {
        EntityId entityId;

        if (!m_FreeEntities.empty())
        {
            entityId = RecycleEntity();
        } else
        {
            entityId = NewEntity();
        }

        if (entityId == s_InvalidEntity)
        {
            SPARK_ASSERT(false, "Max number of entities reached.");
            return s_InvalidEntity;
        }

        m_EntityRecords.insert(std::make_pair(entityId, EntityRecord
                {
                        .archetype = GetArchetype({}),
                        .entity_idx = 0 // not valid for null archetype
                }));

        return entityId;
    }

    EntityId World::NewEntity()
    {
        return m_EntityCounter++;
    };

    EntityId World::RecycleEntity()
    {
        const auto entity = m_FreeEntities.back();
        m_FreeEntities.pop_back();

        return entity;
    }

    void* World::GetComponent(EntityId entity, ComponentId component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        auto compInfo = m_ComponentRecords[component];
        if (compInfo.archetypeRecords.count(archetype->id) == 0)
        {
            SPARK_ASSERT(false);
            return nullptr;
        }
        ArchetypeComponentKey& a_record = compInfo.archetypeRecords[archetype->id];
        return archetype->components[a_record].GetComponent(record.entity_idx);
    }

    void World::AddComponent(EntityId entity, ComponentId component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        Archetype* next_archetype = GetNextArchetype(archetype, component, true);
        record.entity_idx = MoveEntity(archetype, record.entity_idx, next_archetype);
        record.archetype = next_archetype;
    }

    void World::DestroyEntity(EntityId entity)
    {
        if (entity == s_InvalidEntity)
        {
            return;
        }

        for (const auto comp: m_EntityRecords.at(entity).archetype->type)
        {
            RemoveComponent(entity, comp);
        }

        m_EntityRecords.erase(entity);
        m_FreeEntities.push_back(entity);
    }

    bool World::HasComponent(EntityId entity, ComponentId component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        auto compInfo = m_ComponentRecords[component];
        return compInfo.archetypeRecords.count(archetype->id) != 0;
    }

    bool World::HasArchetype(const Type& type)
    {
        return m_ArchetypeTypeLookup.contains(type);
    }

    void World::CreateArchetype(const Type& type)
    {
        Archetype archetype = {
                .id = m_ArchetypeCounter++,
                .type = type,
                .components = {},
                .edges = {}
        };

        archetype.components.reserve(type.size());
        for (const auto& compType: type)
        {
            auto& compInfo = m_ComponentRecords[compType];
            archetype.components.emplace_back(compInfo.type->size);
        }

        m_Archetypes.insert(std::make_pair(archetype.id, archetype));
        m_ArchetypeTypeLookup.insert(std::make_pair(type, archetype.id));

        for (int i = 0; i < type.size(); ++i)
        {
            auto& compInfo = m_ComponentRecords[type[i]];
            compInfo.archetypeRecords.insert(std::make_pair(archetype.id, i));
        }
    }

    void World::RemoveComponent(EntityId entity, ComponentId component)
    {
        EntityRecord& record = m_EntityRecords.at(entity);
        Archetype* archetype = record.archetype;
        Archetype* next_archetype = GetNextArchetype(archetype, component, false);
        record.entity_idx = MoveEntity(archetype, record.entity_idx, next_archetype);
        record.archetype = next_archetype;
    }

    size_t World::MoveEntity(Archetype* archetype, size_t entityIdx, Archetype* nextArchetype)
    {
        std::vector<std::pair<ComponentId, uint8_t*>> compData;
        for (int i = 0; i < archetype->components.size(); ++i)
        {
            ComponentId id = archetype->type[i];
            uint8_t* data = archetype->components[i].GetComponent(entityIdx);

            compData.emplace_back(std::make_pair(id, data));
        }

        // copy to new archetype
        std::set<ComponentId> componentsAccountedFor = {};
        for (auto& data: compData)
        {
            auto compInfo = m_ComponentRecords[data.first];
            if (compInfo.archetypeRecords.count(nextArchetype->id) == 0)
            {
                // not part of new archetype
                continue;
            }
            ArchetypeComponentKey& aRecord = compInfo.archetypeRecords[nextArchetype->id];
            nextArchetype->components[aRecord].AddComponent(data.second);

            componentsAccountedFor.insert(data.first);
        }

        // add missing components
        for (int i = 0; i < nextArchetype->components.size(); ++i)
        {
            if (!componentsAccountedFor.contains(nextArchetype->type[i]))
            {
                auto compInfo = m_ComponentRecords[nextArchetype->type[i]];
                std::vector<uint8_t> tempCompData(compInfo.type->size);
                nextArchetype->components[i].AddComponent(tempCompData.data());
            }
        }

        // remove from og archetype
        for (int i = 0; i < archetype->components.size(); ++i)
        {
            archetype->components[i].RemoveComponent(entityIdx);
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

    Archetype* World::GetNextArchetype(Archetype* archetype, ComponentId component, bool add)
    {
        if (!archetype->edges.contains(component))
        {
            auto addType = archetype->type;
            if (!std::ranges::contains(addType, component))
            {
                addType.push_back(component);
            }

            auto removeType = archetype->type;
            std::erase(removeType, component);
            archetype->edges.insert(std::make_pair(component,
                                                   ArchetypeEdge{
                                                           .add = GetArchetype(addType),
                                                           .remove = GetArchetype(removeType)
                                                   }));
        }

        const auto& edges = archetype->edges.at(component);
        return add ? edges.add : edges.remove;
    }

    Archetype* World::GetArchetype(const Type& type)
    {
        if (!HasArchetype(type))
        {
            CreateArchetype(type);
        }

        return &m_Archetypes.at(m_ArchetypeTypeLookup.at(type));
    }

    void World::Update(float dt)
    {
        for (const auto& updateGroup: m_SystemUpdateGroups)
        {
            std::for_each(std::execution::par,
                          updateGroup.begin(),
                          updateGroup.end(),
                          [this, dt](auto&& systemId)
                          {
                              if (auto* system = m_Systems[systemId])
                              {
                                  system->Update(dt);
                              }
                          });
        }
    }
}