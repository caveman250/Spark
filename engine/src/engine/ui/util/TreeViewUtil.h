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
    ecs::Id CreateTreeView(components::TreeViewComponent** outTreeView, components::RectTransformComponent** outTransform, bool editorOnly);
    ecs::Id InsertTreeNode(const ecs::Id& treeViewEntity,
        components::TreeViewComponent* treeView,
        const ecs::Id& parentNode,
        const String& name,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText,
        bool editorOnly);
}
