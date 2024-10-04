#include <engine/ui/components/ReceivesMouseEventsComponent.h>
#include "TreeViewUtil.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    ecs::Id CreateTreeView(components::TreeViewComponent** outTreeView, components::RectTransformComponent** outTransform)
    {
        auto world = Application::Get()->GetWorld();

        auto entity = world->CreateEntity();
        *outTreeView = world->AddComponent<components::TreeViewComponent>(entity);
        *outTransform = world->AddComponent<components::RectTransformComponent>(entity);
        return entity;
    }

    ecs::Id InsertTreeNode(ecs::Id treeViewEntity,
       components::TreeViewComponent* treeView,
        ecs::Id parentNode,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        treeView->dirty = true;

        auto entity = world->CreateEntity();
        world->AddChild(parentNode, entity);

        *outTreeNode = world->AddComponent<components::TreeNodeComponent>(entity);
        (*outTreeNode)->onCollapsedStateChange.Subscribe<components::TreeViewComponent>(treeViewEntity, [](bool, components::TreeViewComponent* treeView)
        {
            treeView->dirty = true;
        });

        *outText = world->AddComponent<components::TextComponent>(entity);
        (*outText)->font = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
        (*outText)->fontSize = 18;

        world->AddComponent<components::RectTransformComponent>(entity);
        world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
        world->AddComponent<components::WidgetComponent>(entity);

        return entity;
    }
}
