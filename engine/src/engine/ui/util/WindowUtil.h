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
    ecs::Id CreateWindow(RectTransformComponent** transform,
                         WindowComponent** window,
                         TitleBarComponent** titleBar,
                         ecs::Id& childArea,
                         const std::string& title,
                         std::function<void()> onClose,
                         const bool editorOnly);
}
