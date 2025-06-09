#pragma once

#include "spark.h"
#include "engine/ui/Visibility.h"

namespace se::ui::components
{
    struct WidgetComponent : reflect::ObjectBase
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

        SPARK_MEMBER(Serialized)
        bool dirty = false;
    };
}
