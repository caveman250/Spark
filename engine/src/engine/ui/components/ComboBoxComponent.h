#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui::components
{
    struct ComboBoxComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<int> onItemSelected = {};
        bool collapsed = true;
        bool lastCollapsed = false;
        size_t fontSize = 16;
        ecs::Id collapsedEntity = {};
        ecs::Id expandedEntity = {};
    };
}

namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::ComboBoxComponent>(ecs::System* system,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::ComboBoxComponent* context)
    {
        components::RectTransformComponent fakeTransform = {};
        fakeTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        fakeTransform.rect = util::CalculateScreenSpaceRect(fakeTransform, thisRect);

        return GetDesiredSize<ecs::NullComponentType>(system, context->collapsedEntity, fakeTransform, nullptr);
    }
}
