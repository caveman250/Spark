#include "RectTransformObserver.h"

#include "engine/Application.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ecs/relationships/ChildOf.h"

namespace se::ui::observers
{
    void RectTransformObserver::OnAdded(ecs::Id entity, components::RectTransformComponent*)
    {
        auto world = Application::Get()->GetWorld();
        if (!world->HasRelationshipWildcard<ecs::components::ChildOf>(entity) &&
            !world->HasComponent<ecs::components::RootComponent>(entity))
        {
            world->AddComponent<ecs::components::RootComponent>(entity);
        }
    }

    void RectTransformObserver::OnRemoved(ecs::Id, components::RectTransformComponent*)
    {

    }
}
