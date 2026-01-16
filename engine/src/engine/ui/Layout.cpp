#include "Layout.h"
#include "Widgets.generated.h"

namespace se::ui
{
    void Layout::DoLayoutChildren(ecs::World* world,
                                     ecs::System* system,
                                     const ecs::Id& entity,
                                     components::RectTransformComponent& thisRect)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
        system->RunChildQuery(entity, dec,
          [world, system, &thisRect, entity](const ecs::SystemUpdateData& updateData)
          {
              std::visit([world, system, updateData, thisRect, entity](auto&& value)
              {
                  const auto& entities = updateData.GetEntities();
                  auto transforms = updateData.GetComponentArray<components::RectTransformComponent>();

                  for (size_t i = 0; i < entities.size(); ++i)
                  {
                      auto& transform = transforms[i];
                      transform.rect = util::CalculateScreenSpaceRect(transform, thisRect);
                      transform.layer = thisRect.layer + 1;
                      if (!transform.overridesChildSizes)
                      {
                          LayoutWidgetChildren(world, system, entities[i], transforms[i], value);
                          transform.needsLayout = false;
                      }
                      else
                      {
                          transform.needsLayout = true;
                      }
                  }
              }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());

              return false;
          });
    }
}
