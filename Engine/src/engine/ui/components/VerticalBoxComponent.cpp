#include <Widgets.generated.h>
#include "VerticalBoxComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(VerticalBoxComponent)
    DEFINE_SPARK_COMPONENT_END(VerticalBoxComponent)
}

namespace se::ui
{
    se::math::Vec2 GetVerticalBoxChildrenDesiredSize(ecs::BaseSystem *system, const ecs::Id &entity, const Rect& parentRect)
    {
        se::math::Vec2 desiredSize = {};
        system->RunVariantChildQuery<SPARK_CONST_WIDGET_TYPES>(entity,
           (std::function<bool(const ecs::Id&, std::variant<SPARK_WIDGET_CONST_POINTER_TYPES>)>)
                   [parentRect, &desiredSize, system](const ecs::Id& entity, std::variant<SPARK_WIDGET_CONST_POINTER_TYPES> var)
                   {
                      std::visit([parentRect, entity, &desiredSize, system](auto&& value)
                      {
                          math::Vec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, entity, parentRect, value);
                          desiredSize.x = std::max(childDesiredSize.x, desiredSize.x);
                          desiredSize.y += childDesiredSize.y;
                      }, var);
                       return false;
                   });
        return desiredSize;
    }
}
