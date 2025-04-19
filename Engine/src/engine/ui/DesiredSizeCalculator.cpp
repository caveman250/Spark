#include "DesiredSizeCalculator.h"
#include "Widgets.generated.h"

se::math::Vec2 se::ui::DesiredSizeCalculator::GetChildrenDesiredSize(se::ecs::BaseSystem *system,
                                                                     const ecs::Id& entity,
                                                                     const se::ui::Rect& parentRect)
{
    math::Vec2 ret = {};
    system->RunVariantChildQuery<SPARK_CONST_WIDGET_TYPES>(entity, (std::function<bool(const ecs::Id&, ConstWidgetVariant)>)[&ret, system, entity, parentRect](const ecs::Id&, ConstWidgetVariant var)
    {
        std::visit([&ret, system, entity, parentRect](auto&& value)
        {
            // TODO calc parent rect here.
            ret = std::max(ret, DesiredSizeCalculator::GetDesiredSize(system, entity, parentRect, value));
        }, var);
        return false;
    });

    return ret;
}
