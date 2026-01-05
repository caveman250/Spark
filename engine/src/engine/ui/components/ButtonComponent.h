#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Object.h"

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

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> image = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> pressedImage = {};

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> hoveredImage = {};

        ecs::Signal<> onPressed;
        ecs::Signal<> onReleased;

    private:
        bool lastPressed = false;
        bool lastHovered = false;

        friend class systems::ButtonSystem;
    };
}