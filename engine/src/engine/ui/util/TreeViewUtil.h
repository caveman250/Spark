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
    ecs::Id InsertTreeNode(ecs::Id treeViewEntity,
        components::TreeViewComponent* treeView,
        ecs::Id parentNode,
        const String& name,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText,
        bool editorOnly);
}
