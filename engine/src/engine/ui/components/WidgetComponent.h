#pragma once

#include "spark.h"
#include "engine/ui/Visibility.h"

namespace se::ui::components
{
    struct WidgetComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT()

        Visibility visibility = Visibility::Visible;
        Visibility parentVisibility = Visibility::Visible;
        bool updateEnabled = true;
        bool parentUpdateEnabled = true;
        bool dirty = false;
    };
}
