#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::systems
{
    class ButtonSystem;
}

namespace se::ui::components
{
    struct ButtonComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        bool pressed = false;
        bool hovered = false;

        std::shared_ptr<asset::Texture> image = nullptr;
        std::shared_ptr<asset::Texture> pressedImage = nullptr;
        std::shared_ptr<asset::Texture> hoveredImage = nullptr;

        ecs::Signal<> onPressed;
        ecs::Signal<> onReleased;

    private:
        bool lastPressed = false;
        bool lastHovered = false;

        friend class systems::ButtonSystem;
    };
}