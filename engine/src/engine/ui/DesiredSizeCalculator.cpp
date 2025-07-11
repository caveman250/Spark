#include "DesiredSizeCalculator.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "Widgets.generated.h"

se::math::IntVec2 se::ui::DesiredSizeCalculator::GetChildrenDesiredSize(ecs::System* system,
                                                                     const ecs::Id& entity,
                                                                     components::RectTransformComponent& thisRect)
{
    math::IntVec2 ret = {};
    auto dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<const components::WidgetComponent>()
            .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
    system->RunChildQuery(entity,dec,
      [&ret, system, &thisRect, entity](const ecs::SystemUpdateData& updateData)
    {
        std::visit([&ret, system, updateData, thisRect, entity](auto&& value)
        {
            const auto& entities = updateData.GetEntities();
            auto transforms = updateData.GetComponentArray<components::RectTransformComponent>();
            auto widgets = updateData.GetComponentArray<const components::WidgetComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto desired = DesiredSizeCalculator::GetDesiredSize(system, entities[i], &widgets[i], thisRect, transforms[i], value);
                ret.x = std::max(ret.x, desired.x);
                ret.y = std::max(ret.y, desired.y);
            }

        }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
        return false;
    });

    return ret;
}
