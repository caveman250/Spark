#include "ecs_fwd.h"
#include "engine/Application.h"

namespace se::ecs
{
    Id::Id(uint64_t _id)
        : id(_id)
        , name(Application::Get()->GetWorld()->GetName(id))
    {
    }

    Id::Id(const Id& _id)
        : id(_id.id)
        , name(_id.name)
    {

    }

    Id::Id(Id&& _id)
        : id(_id.id)
        , name(_id.name)
    {
    }

    Id& Id::operator=(const Id& rhs)
    {
        id = rhs.id;
        name = Application::Get()->GetWorld()->GetName(id);
        return *this;
    }
}
