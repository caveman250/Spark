#pragma once

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent;
    struct ScrollViewComponent;
}

namespace se::ui::util
{
    struct NewScrollBox
    {
        components::ScrollBoxComponent* scrollBox = nullptr;
        components::ScrollViewComponent* scrollView = nullptr;
        components::RectTransformComponent* transform = nullptr;
        ecs::Id scrollBoxEntity = ecs::s_InvalidEntity;
        ecs::Id scrollViewEntity = ecs::s_InvalidEntity;
        ecs::Id scrollBarEntity = ecs::s_InvalidEntity;
    };

    NewScrollBox CreateScrollBox(const ecs::Id& scene);
}
