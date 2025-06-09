#include <Widgets.generated.h>
#include "HorizontalBoxComponent.h"

namespace se::ui
{
    math::IntVec2 GetHorizontalBoxChildrenDesiredSize(ecs::System* system,
                                                        const ecs::Id& entity,
                                                        const components::RectTransformComponent& thisRect,
                                                        const components::HorizontalBoxComponent* context)
    {
        math::IntVec2 desiredSize = { 0, static_cast<int>(context->paddingTop + context->paddingBottom) };
        auto dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<const components::WidgetComponent>()
            .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, dec,
           [context, thisRect, &desiredSize, system](const ecs::SystemUpdateData& updateData)
           {
              std::visit([context, thisRect, updateData, &desiredSize, system](auto&& value)
              {
                  const auto& entities = updateData.GetEntities();
                  auto transforms = updateData.GetComponentArray<components::RectTransformComponent>();
                  auto widgets = updateData.GetComponentArray<const components::WidgetComponent>();

                  for (size_t i = 0; i < entities.size(); ++i)
                  {
                      auto& transform = transforms[i];
                      transform.minX = transform.maxX = transform.minY = transform.maxY = 0;
                      math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system,
                                                                                          entities[i],
                                                                                          &widgets[i],
                                                                                          thisRect,
                                                                                          transform,
                                                                                          value);
                      // undo anchor offsets, as we override those anyway and don't want them to apply recursivly.
                      childDesiredSize -= DesiredSizeCalculator::CalculateAnchorOffsets(transform, thisRect.rect);
                      desiredSize.y = std::max(childDesiredSize.y + static_cast<int>(context->paddingTop + context->paddingBottom), desiredSize.y);
                      desiredSize.x += childDesiredSize.x + context->spacing;
                  }
              }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
               return false;
           });
        return desiredSize;
    }
}
