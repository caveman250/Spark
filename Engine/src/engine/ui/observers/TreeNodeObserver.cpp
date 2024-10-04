#include "TreeNodeObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void TreeNodeObserver::OnAdded(ecs::Id entity, components::TreeNodeComponent*)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        if (!world->HasComponent<components::ReceivesMouseEventsComponent>(entity))
        {
            world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
        }

        if (!world->HasComponent<components::TextComponent>(entity))
        {
            auto text = world->AddComponent<components::TextComponent>(entity);
            text->font = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
            text->fontSize = 18;
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
