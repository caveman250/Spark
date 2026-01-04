#include "ecs_fwd.h"
#include "engine/Application.h"

namespace se::ecs
{
    Id::Id(uint64_t _id)
        : id(_id)
        , name(Application::Get()->GetWorld() ? Application::Get()->GetWorld()->GetName(id) : nullptr)
        , flags(Application::Get()->GetWorld() ? Application::Get()->GetWorld()->GetFlags(id) : nullptr)
        , scene(Application::Get()->GetWorld() ? Application::Get()->GetWorld()->GetScene(id) : nullptr)
    {
    }

    Id::Id(const Id& _id)
        : id(_id.id)
        , name(_id.name)
        , flags(_id.flags)
        , scene(_id.scene)
    {

    }

    Id::Id(Id&& _id)
        : id(_id.id)
        , name(_id.name)
        , flags(_id.flags)
        , scene(_id.scene)
    {
    }

    Id& Id::operator=(const Id& rhs)
    {
        id = rhs.id;
        name = rhs.name;
        flags = rhs.flags;
        scene = rhs.scene;
        return *this;
    }
}
