#pragma once

#include "spark.h"

namespace se::ui::components
{
    enum class SplitViewDirection;
}

namespace se::ui::util
{
    ecs::Id AddSplitView(const ecs::Id& scene,
        const ecs::Id& window1,
        const ecs::Id& window2,
        const components::SplitViewDirection dir,
        const float sliderVal);
}
