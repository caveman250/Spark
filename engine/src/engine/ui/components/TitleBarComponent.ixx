module;

#include "spark.h"
#include "engine/reflect/Object.h"
#include "engine/ecs/Component.h"

export module TitleBarComponent;
import Signal;
import TitleBarSystem;
import Material;

namespace se::ui::components
{
    export struct TitleBarComponent : ecs::Component
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