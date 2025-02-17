#include "Relationship.h"

#include "engine/bits/PackUtil.h"

namespace se::ecs
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Relationship)
        DEFINE_SERIALIZED_MEMBER(m_Id)
    DEFINE_SPARK_CLASS_END(Relationship)

    uint64_t Relationship::GetId() const
    {
        return m_Id;
    }

    void Relationship::SetId(uint64_t id)
    {
        m_Id = id;
    }
}
