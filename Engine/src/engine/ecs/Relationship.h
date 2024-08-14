#pragma once

#include "RelationshipComponent.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    typedef uint64_t Id;

    class Relationship
    {
    public:
        DECLARE_SPARK_POD_CLASS(Relationship)

        Relationship() = default;
        Id GetId() const;
        void SetId(Id id);
    private:
        Id m_Id;
    };

    template <typename T>
    Relationship CreateEntityRelationship(Id entity)
    {
        static_assert(std::is_base_of_v<RelationshipComponent, T>, "Relationship components must inherit from RelationshipComponent.");
        Relationship relationship;
        relationship.SetId(bits::PackUtil::Pack64(bits::PackUtil::UnpackA64(T::GetComponentId()), bits::PackUtil::UnpackA64(entity)));
        return relationship;
    }

    template <typename T, typename Y>
    Relationship CreateComponentRelationship()
    {
        static_assert(std::is_base_of_v<RelationshipComponent, T>, "Relationship components must inherit from RelationshipComponent.");
        Relationship relationship;
        relationship.SetId(bits::PackUtil::Pack64(bits::PackUtil::UnpackA64(T::GetComponentId()), bits::PackUtil::UnpackA64(Y::GetComponentId())));
        return relationship;
    }
}
