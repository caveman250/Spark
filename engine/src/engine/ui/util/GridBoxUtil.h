#pragma once

#include "engine/math/math.h"

namespace se::ui::components
{
    struct RectTransformComponent;
}

namespace se::ecs
{
    class System;
}

namespace se::ui::util
{
    math::IntVec2 GetGridBoxItemSize(const ecs::Id& entity,
                                     ecs::System* system,
                                     const components::RectTransformComponent& rectTransform);
}
