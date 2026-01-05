#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui::components
{
    SPARK_ENUM()
    enum class SplitViewDirection
    {
        Vertical,
        Horizontal
    };

    struct SplitViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Id entity1 = {};
        ecs::Id sliderEntity = {};
        ecs::Id entity2 = {};
        float sliderPos = .5f;
        float lastSliderPos = {};
        bool isResizing = false;
        bool isShowingResizeCursor = false;
        SplitViewDirection dir = SplitViewDirection::Horizontal;
    };
}

namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::SplitViewComponent>(ecs::System* system,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::SplitViewComponent* context)
    {
        math::Vec2 desiredSize = {};
        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent titleRectTransform = {};
        titleRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRectTransform.rect = util::CalculateScreenSpaceRect(titleRectTransform, thisRect);

        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent contentsRectTransform = {};
        contentsRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        contentsRectTransform.rect = util::CalculateScreenSpaceRect(contentsRectTransform, thisRect);

        auto entity1size = GetDesiredSize<ecs::NullComponentType>(system, context->entity1, nullptr, thisRect, titleRectTransform, nullptr);
        auto entity2size = GetDesiredSize<ecs::NullComponentType>(system, context->entity2, nullptr, thisRect, contentsRectTransform, nullptr);

        desiredSize = math::Vec2(static_cast<float>(std::max(entity1size.x, entity2size.x)), static_cast<float>(entity1size.y + entity2size.y) + 5.f);

        return desiredSize;
    }
}
