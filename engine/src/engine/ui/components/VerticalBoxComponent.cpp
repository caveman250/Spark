#include <Widgets.generated.h>
#include "VerticalBoxComponent.h"
#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    math::IntVec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system,
                                                     const ecs::Id& entity,
                                                     const components::RectTransformComponent& thisRect,
                                                     const components::VerticalBoxComponent* context)
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
                      desiredSize.x = std::max(childDesiredSize.x + static_cast<int>(context->paddingLeft + context->paddingRight), desiredSize.x);
                      desiredSize.y += childDesiredSize.y + static_cast<int>(context->spacing);
                  }
              }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());
               return false;
           });
        return desiredSize;
    }

    template <>
    void Layout::LayoutWidgetChildren<components::VerticalBoxComponent>(ecs::World* world,
                                                     ecs::System* system,
                                                     const ecs::Id& entity,
                                                     components::RectTransformComponent& verticalBoxTransform,
                                                     components::VerticalBoxComponent* verticalBox)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<components::WidgetComponent>();
        std::unordered_map<ecs::Id, util::ChildDesiredSizeInfo> childRects = { };

        util::ForEachWidgetChild(entity, system, dec,
         [world, system, verticalBoxTransform, verticalBox, &childRects](const ecs::SystemUpdateData& updateData,
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

                 math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, &widgets[i], verticalBoxTransform, rect, &widgetComps[i]);
                 childRects[child] = util::ChildDesiredSizeInfo(&rect, childDesiredSize);
             }

             return false;
         });

        auto window = Application::Get()->GetWindow();
        float currY = verticalBox->paddingTop * window->GetContentScale();
        for (const auto& child: world->GetChildren(entity))
        {
            auto desiredSizeInfo = childRects.at(child);
            desiredSizeInfo.rectTransform->rect.topLeft = verticalBoxTransform.rect.topLeft + math::Vec2 { verticalBox->paddingLeft, currY };
            desiredSizeInfo.rectTransform->rect.size = desiredSizeInfo.desiredSize;
            desiredSizeInfo.rectTransform->rect.size.x = verticalBoxTransform.rect.size.x - static_cast<int>(verticalBox->paddingRight - verticalBox->paddingLeft);
            currY += static_cast<float>(desiredSizeInfo.desiredSize.y) + verticalBox->spacing;

            desiredSizeInfo.rectTransform->layer = verticalBoxTransform.layer;
        }

        verticalBoxTransform.rect.size.y = static_cast<int>(currY + verticalBox->paddingBottom * window->GetContentScale());
        verticalBoxTransform.maxY = verticalBoxTransform.minY + currY / window->GetContentScale() + verticalBox->paddingBottom;

        dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>();
        util::ForEachWidgetChild(entity, system, dec,
         [world, system](const ecs::SystemUpdateData& updateData,
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

        verticalBoxTransform.needsLayout = false;
    }
}
