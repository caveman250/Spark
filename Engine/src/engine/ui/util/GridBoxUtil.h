#pragma once

#include "engine/math/math.h"

namespace se::ecs
{
    class System;
}

namespace se::ui::util
{
    math::IntVec2 GetGridBoxItemSize(const ecs::Id& entity,
                                     ecs::System* system,
                                     const ui::components::RectTransformComponent& rectTransform);
}
