#pragma once

#include "RelationshipComponent.h"
#include "spark.h"
#include "World.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    typedef uint64_t RelationshipId;

    class Relationship
    {
    public:
        DECLARE_SPARK_POD_CLASS(Relationship)

        Relationship() = default;
        RelationshipId GetId() const;
        void SetId(RelationshipId id);
    private:
        Relationship(EntityId entity);
        RelationshipId m_Id;
    };

    template <typename T>
    Relationship CreateRelationship(EntityId entity)
    {
        static_assert(std::is_base_of_v<RelationshipComponent, T>, "Relationship components must inherit from RelationshipComponent.");
        Relationship relationship;
        relationship.SetId(bits::PackUtil::Pack64(bits::PackUtil::UnpackA64(T::GetComponentId()), bits::PackUtil::UnpackA64(entity)));
        return relationship;
    }
}
