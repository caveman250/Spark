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
    ecs::Id CreateTreeView(components::TreeViewComponent** outTreeView, components::RectTransformComponent** outTransform, const ecs::Id& scene);
    ecs::Id InsertTreeNode(const ecs::Id& treeViewEntity,
        components::RectTransformComponent* treeViewRect,
        const ecs::Id& parentNode,
        const std::string& name,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText,
        const ecs::Id& scene);
}
