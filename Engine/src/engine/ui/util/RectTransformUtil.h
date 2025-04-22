#pragma once

#include "engine/ecs/World.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::util
{
    using namespace se::ui::components;
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform);

    void LayoutChildren(ecs::World* world, ecs::System* system, ecs::Id entity, const components::RectTransformComponent& parentRect, int depth);
}
