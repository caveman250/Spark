#include <Widgets.generated.h>
#include "VerticalBoxComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(VerticalBoxComponent)
    DEFINE_SPARK_COMPONENT_END(VerticalBoxComponent)
}

namespace se::ui
{
    se::math::Vec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system, const ecs::Id& entity, const Rect& parentRect)
    {
        se::math::Vec2 desiredSize = {};
        auto dec = ecs::ChildQueryDeclaration()
            .WithVariantComponent<SPARK_CONST_WIDGET_TYPES>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
           [parentRect, entity, &desiredSize, system](const ecs::SystemUpdateData& updateData)
           {
              std::visit([parentRect, entity, &desiredSize, system](auto&& value)
              {
                  math::Vec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, entity, parentRect, value);
                  desiredSize.x = std::max(childDesiredSize.x, desiredSize.x);
                  desiredSize.y += childDesiredSize.y;
              }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES>());
               return false;
           });
        return desiredSize;
    }
}
