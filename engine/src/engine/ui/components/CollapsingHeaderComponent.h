#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui::components
{
    struct CollapsingHeaderComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<bool> onCollapsed = {};
        bool collapsed = true;
        bool lastCollapsed = false;
        size_t fontSize = 16;
        ecs::Id titleEntity = {};
        ecs::Id contentsEntity = {};
    };
}

namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::CollapsingHeaderComponent>(ecs::System* system,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::CollapsingHeaderComponent* context)
    {
        math::Vec2 desiredSize = {};
        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent titleRectTransform = {};
        titleRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRectTransform.rect = util::CalculateScreenSpaceRect(titleRectTransform, thisRect);
        if (context->collapsed)
        {
            desiredSize = GetDesiredSize<ecs::NullComponentType>(system, context->titleEntity, nullptr, thisRect, titleRectTransform, nullptr);
        }
        else
        {
            // make a fake rect transform for the title since we cant access the real component here
            components::RectTransformComponent contentsRectTransform = {};
            contentsRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentsRectTransform.rect = util::CalculateScreenSpaceRect(contentsRectTransform, thisRect);

            auto headerSize = GetDesiredSize<ecs::NullComponentType>(system, context->titleEntity, nullptr, thisRect, titleRectTransform, nullptr);
            auto contentSize = GetDesiredSize<ecs::NullComponentType>(system, context->contentsEntity, nullptr, thisRect, contentsRectTransform, nullptr);

            desiredSize = math::Vec2(std::max(headerSize.x, contentSize.x), headerSize.y + contentSize.y + 5.f);
        }

        return desiredSize;
    }
}
