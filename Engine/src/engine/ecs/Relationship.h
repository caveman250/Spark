#pragma once

#include "ecs_fwd.h"
#include "RelationshipComponent.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    class Relationship
    {
    public:
        DECLARE_SPARK_POD_CLASS(Relationship)

        Relationship() = default;
        uint64_t GetId() const;
        void SetId(uint64_t id);
    private:
        uint64_t m_Id;
    };
}
