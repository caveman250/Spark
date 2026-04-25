#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/Signal.h"
#include "engine/ecs/Component.h"
#include "engine/asset/AssetReference.h"
#include "engine/input/InputComponent.h"

namespace se::ui::systems
{
    class ButtonSystem;
}

namespace se::ui::components
{
    struct ButtonComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool pressed = false;
        bool hovered = false;
        input::MouseButton pressedButton = {};
        std::time_t timePressed = 0;

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> image = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> pressedImage = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> hoveredImage = {};

        SPARK_MEMBER(Serialized)
        ecs::Signal<input::MouseButton> onPressed = {};

        SPARK_MEMBER(Serialized)
        ecs::Signal<input::MouseButton> onDoubleClick = {};

        math::IntVec2 pressedPosition = {};
        bool isDragging = false;

        ecs::Signal<input::MouseButton> onDragged = {};

        ecs::Signal<input::MouseButton> onReleased = {};

    private:
        bool lastPressed = false;
        bool lastHovered = false;

        friend class systems::ButtonSystem;
    };
}
