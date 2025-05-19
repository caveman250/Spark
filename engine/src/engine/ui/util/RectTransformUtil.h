#pragma once

#include "engine/ecs/World.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::util
{
    using namespace se::ui::components;
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform);
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const Rect& parentRect);

    void LayoutChildren(ecs::World* world, ecs::System* system, ecs::Id entity, const components::RectTransformComponent& parentRect, int depth);

    struct ChildDesiredSizeInfo
    {
        ui::components::RectTransformComponent* rectTransform = nullptr;
        math::IntVec2 desiredSize = {};
    };
    std::unordered_map<ecs::Id, ChildDesiredSizeInfo> GetChildrenDesiredSizes(const ecs::Id& entity,
                                                                              ecs::System* system,
                                                                              const ui::components::RectTransformComponent& transform);
    std::unordered_map<ecs::Id, RectTransformComponent*> GetChildRectTransforms(const ecs::Id& entity, ecs::System* system);

    void TranslateChildren(const ecs::Id& entity, ecs::System* system, const math::IntVec2& delta);
}
