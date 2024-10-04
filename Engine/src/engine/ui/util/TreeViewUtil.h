#pragma once
#include "engine/ecs/World.h"

namespace se::ui::components
{
    struct TreeViewComponent;
    struct RectTransformComponent;
    struct TextComponent;
    struct TreeNodeComponent;
}

namespace se::ui::util
{
    ecs::Id CreateTreeView(components::TreeViewComponent** outTreeView, components::RectTransformComponent** outTransform);
    ecs::Id InsertTreeNode(ecs::Id treeViewEntity,
        components::TreeViewComponent* treeView,
        ecs::Id parentNode,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText);
}
