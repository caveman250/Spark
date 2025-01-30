#pragma once

#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::util
{
    using namespace se::ui::components;
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform);
}
