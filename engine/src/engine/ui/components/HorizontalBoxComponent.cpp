#include <Widgets.generated.h>
#include "HorizontalBoxComponent.h"

#include "engine/ui/util/WidgetUtil.h"

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

    template<>
    void Layout::LayoutWidgetChildren<components::HorizontalBoxComponent>(ecs::World* world,
                                                                          ecs::System* system,
                                                                          const ecs::Id& entity,
                                                                          components::RectTransformComponent& horizontalBoxTransform,
                                                                          components::HorizontalBoxComponent* horizontalBox)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<components::WidgetComponent>();
        std::unordered_map<ecs::Id, util::ChildDesiredSizeInfo> childRects = { };

        util::ForEachWidgetChild(entity, system, dec,
         [world, system, horizontalBoxTransform, horizontalBox, &childRects](const ecs::SystemUpdateData& updateData,
                                                        auto&& widgetComps)
         {
             const auto& children = updateData.GetEntities();
             auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();
             auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

             for (size_t i = 0; i < children.size(); ++i)
             {
                 const auto& child = children[i];
                 auto& rect = rects[i];
                 rect.anchors = { 0.f, 1.f, 0.f, 0.f };
                 rect.minY = 0;
                 rect.maxY = 0;
                 rect.minX = 0;
                 rect.maxX = 0;

                 math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, &widgets[i], horizontalBoxTransform, rect, &widgetComps[i]);
                 childRects[child] = util::ChildDesiredSizeInfo(&rect, childDesiredSize);
             }

             return false;
         });

        auto window = Application::Get()->GetWindow();
        float currX = horizontalBox->paddingLeft * window->GetContentScale();
        for (const auto& child: world->GetChildren(entity))
        {
            auto desiredSizeInfo = childRects.at(child);
            desiredSizeInfo.rectTransform->rect.topLeft = horizontalBoxTransform.rect.topLeft + math::Vec2 { currX, horizontalBox->paddingTop };
            desiredSizeInfo.rectTransform->rect.size = desiredSizeInfo.desiredSize;
            desiredSizeInfo.rectTransform->rect.size.y = horizontalBoxTransform.rect.size.y - static_cast<int>(horizontalBox->paddingBottom - horizontalBox->paddingTop);
            currX += static_cast<float>(desiredSizeInfo.desiredSize.x) + horizontalBox->spacing;

            desiredSizeInfo.rectTransform->layer = horizontalBoxTransform.layer;
        }

        horizontalBoxTransform.rect.size.x = static_cast<int>(currX + horizontalBox->paddingRight * window->GetContentScale());
        horizontalBoxTransform.maxX = horizontalBoxTransform.minX + currX / window->GetContentScale() + horizontalBox->paddingRight;

        dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>();
        util::ForEachWidgetChild(entity, system, dec,
         [world, system, horizontalBoxTransform, horizontalBox, &childRects](const ecs::SystemUpdateData& updateData,
                                                        auto&& widgetComps)
         {
             const auto& children = updateData.GetEntities();
             auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

             for (size_t i = 0; i < children.size(); ++i)
             {
                 const auto& child = children[i];
                 auto& rect = rects[i];
                 if (!rect.overridesChildSizes)
                 {
                     LayoutWidgetChildren(world,
                                          system,
                                          child,
                                          rect,
                                          &widgetComps[i]);
                     rect.needsLayout = false;
                 }
                 else
                 {
                     rect.needsLayout = true;
                 }
             }

             return false;
         });

        horizontalBoxTransform.needsLayout = false;
    }
}
