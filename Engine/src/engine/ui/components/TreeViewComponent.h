#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct TreeViewComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(TreeViewComponent)

        bool dirty = false;
        ui::Rect lastRect = {};
    };
}
