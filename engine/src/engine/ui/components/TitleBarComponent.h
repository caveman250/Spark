#pragma once

#include "spark.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Object.h"
#include "engine/render/Material.h"

namespace se::ui::systems
{
    class TitleBarSystem;
}

namespace se::ui::components
{
    struct TitleBarComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool pressed = false;
        ecs::Signal<float, float> onMove = {};

        std::shared_ptr<render::Material> material = nullptr;

    private:
        bool lastPressed = false;

        friend class systems::TitleBarSystem;
    };
}