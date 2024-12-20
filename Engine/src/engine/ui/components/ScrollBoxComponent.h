#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    class ScrollBoxComponent : public reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ScrollBoxComponent)

        float scroll_amount = 0.f;
    };
}
