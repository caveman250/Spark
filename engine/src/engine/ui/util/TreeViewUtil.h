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

    struct TreeNodeParams
    {
        const ecs::Id& treeViewEntity;
        const ecs::Id& parentNode;
        const std::string& name;
        const ecs::Id& scene;
        components::RectTransformComponent* treeViewRect = nullptr;
        std::vector<std::string> contextOptions = {};
        std::function<void(int)> onContextMenuOptionSelected = nullptr;
    };
    struct NewTreeNode
    {
        ecs::Id entity;
        components::TreeNodeComponent* treeNode;
        components::TextComponent* text;
    };
    NewTreeNode InsertTreeNode(const TreeNodeParams& params);
}
