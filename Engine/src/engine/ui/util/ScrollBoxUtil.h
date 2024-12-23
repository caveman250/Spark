#pragma once

namespace se::ui::components
{
    struct ScrollBoxComponent;
}

namespace se::ui::util
{
    ecs::Id CreateScrollBox(components::ScrollBoxComponent** outScrollBox, components::RectTransformComponent** outTransform);
}
