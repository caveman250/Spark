#include "Layout.h"
#include "Widgets.generated.h"

namespace se::ui
{
    void Layout::DoLayoutChildren(ecs::World* world,
                                     ecs::System* system,
                                     const ecs::Id& entity,
                                     int layer,
                                     components::RectTransformComponent& thisRect)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithVariantComponent<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Mutable);
        system->RunChildQuery(entity, dec,
          [world, system, &thisRect, entity, layer](const ecs::SystemUpdateData& updateData)
          {
              std::visit([world, system, updateData, thisRect, entity, layer](auto&& value)
              {
                  const auto& child = updateData.GetEntity();
                  auto transform = updateData.GetComponentArray<components::RectTransformComponent>();

                  transform->rect = util::CalculateScreenSpaceRect(*transform, thisRect);
                  transform->layer = layer;
                  LayoutWidgetChildren(world, system, child, *transform, layer, value);
                  transform->needsLayout = false;
              }, updateData.GetVariantComponentArray<SPARK_WIDGET_TYPES_WITH_NULLTYPE>());

              return false;
          });
    }
}
