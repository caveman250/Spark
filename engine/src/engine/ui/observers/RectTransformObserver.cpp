#include "RectTransformObserver.h"

#include "engine/Application.h"
#include "engine/ecs/components/RootComponent.h"

namespace se::ui::observers
{
    void RectTransformObserver::OnAdded(const ecs::Id& entity, components::RectTransformComponent*)
    {
        auto world = Application::Get()->GetWorld();
        if (!world->HasComponent<ecs::components::RootComponent>(entity) &&
            world->GetParent(entity) == ecs::s_InvalidEntity)
        {
            world->AddComponent<ecs::components::RootComponent>(entity);
        }
    }

    void RectTransformObserver::OnRemoved(const ecs::Id&, components::RectTransformComponent*)
    {

    }
}
