#include "Relationship.h"

#include "engine/bits/PackUtil.h"

namespace se::ecs
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Relationship)
        DEFINE_MEMBER(m_Id)
    DEFINE_SPARK_CLASS_END()

    Id Relationship::GetId() const
    {
        return m_Id;
    }

    void Relationship::SetId(Id id)
    {
        m_Id = id;
    }
}
