#pragma once
#include "spark.h"
#include "engine/ecs/World.h"

namespace se::ui::components
{
    struct TitleBarComponent;
    struct RectTransformComponent;
    struct WindowComponent;
}

namespace se::ui::util
{
    ecs::Id CreateWindow(components::RectTransformComponent** transform,
                         components::WindowComponent** window,
                         components::TitleBarComponent** titleBar,
                         ecs::Id& childArea);
}
