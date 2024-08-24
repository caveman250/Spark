#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::button
{
    class ButtonSubscription;
}

namespace se::ui::systems
{
    class ButtonSystem;
}

namespace se::ui::components
{
    struct ButtonComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ButtonComponent)

        bool pressed = false;
        bool hovered = false;

        std::shared_ptr<asset::Texture> image = nullptr;
        std::shared_ptr<asset::Texture> pressedImage = nullptr;
        std::shared_ptr<asset::Texture> hoveredImage = nullptr;

    private:
        bool lastPressed = false;
        bool lastHovered = false;
        std::vector<std::shared_ptr<button::ButtonSubscription>> subscriptions = {};

        friend class systems::ButtonSystem;
        friend class button::ButtonSubscription;
    };
}
