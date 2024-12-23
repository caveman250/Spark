#pragma once

namespace se::ui::util
{
    using namespace components;
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform);
}
