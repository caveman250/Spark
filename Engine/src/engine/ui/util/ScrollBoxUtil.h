#pragma once

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent;
    struct ScrollViewComponent;
}

namespace se::ui::util
{
    ecs::Id CreateScrollBox(components::ScrollBoxComponent** outScrollBox,
                            ecs::Id& scrollViewEntity,
                            components::ScrollViewComponent** outScrollView,
                            components::RectTransformComponent** outTransform,
                            ecs::Id& scrollBarEntity,
                            bool editorOnly);
}
