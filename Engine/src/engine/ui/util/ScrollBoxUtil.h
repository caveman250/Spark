#pragma once

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent;
}

namespace se::ui::util
{
    ecs::Id CreateScrollBox(components::ScrollBoxComponent** outScrollBox, components::RectTransformComponent** outTransform, bool editorOnly);
}
