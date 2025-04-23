#include "TreeNodeObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void TreeNodeObserver::OnAdded(ecs::Id entity, components::TreeNodeComponent*)
    {
        auto world = Application::Get()->GetWorld();
        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<ui::components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void TreeNodeObserver::OnRemoved(ecs::Id, components::TreeNodeComponent*)
    {

    }
}
