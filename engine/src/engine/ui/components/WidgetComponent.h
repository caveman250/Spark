#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/ui/Visibility.h"

namespace se::ui::components
{
    struct WidgetComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        Visibility visibility = Visibility::Visible;

        SPARK_MEMBER()
        Visibility parentVisibility = Visibility::Visible;

        SPARK_MEMBER(Serialized)
        bool updateEnabled = true;

        SPARK_MEMBER()
        bool parentUpdateEnabled = true;

        SPARK_MEMBER()
        bool dirty = true;
    };
}
