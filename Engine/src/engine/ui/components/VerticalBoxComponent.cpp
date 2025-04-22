#include <Widgets.generated.h>
#include "VerticalBoxComponent.h"

namespace se::ui::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(VerticalBoxComponent)
    DEFINE_SPARK_COMPONENT_END(VerticalBoxComponent)
}

namespace se::ui
{
    se::math::IntVec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system,
                                                     const ecs::Id& entity,
                                                     const ui::components::RectTransformComponent& thisRect,
                                                     const components::VerticalBoxComponent* context)
    {
        se::math::IntVec2 desiredSize = {};
        auto dec = ecs::ChildQueryDeclaration()
            .WithComponent<const ui::components::RectTransformComponent>()
            .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
           [context, thisRect, &desiredSize, system](const ecs::SystemUpdateData& updateData)
           {
              std::visit([context, thisRect, updateData, &desiredSize, system](auto&& value)
              {
                  const auto& entities = updateData.GetEntities();
                  const auto& transforms = updateData.GetComponentArray<const ui::components::RectTransformComponent>();

                  for (size_t i = 0; i < entities.size(); ++i)
                  {
                      math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system,
                                                                                          entities[i],
                                                                                          thisRect,
                                                                                          transforms[i],
                                                                                          value);
                      desiredSize.x = std::max(childDesiredSize.x, desiredSize.x);
                      desiredSize.y += childDesiredSize.y + context->spacing;
                  }
              }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
               return false;
           });
        return desiredSize;
    }
}
