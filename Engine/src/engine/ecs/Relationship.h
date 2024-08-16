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
}
