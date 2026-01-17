#include "Widgets.generated.h"
#include "engine/ui/util/GridBoxUtil.h"
#include "GridBoxComponent.h"

namespace se::ui
{
    math::IntVec2 GetGridBoxChildrenDesiredSize(ecs::System* system,
                                                const ecs::Id& entity,
                                                const components::RectTransformComponent& thisRect,
                                                const components::GridBoxComponent* context)
    {
        auto app = Application::Get();
        auto window = app->GetWindow();
        auto world = app->GetWorld();
        math::IntVec2 itemSize = util::GetGridBoxItemSize(entity, system, thisRect);

        math::IntVec2 desiredSize = { context->spacing, context->spacing };
        auto numChildren = world->GetChildren(entity).size();
        math::IntVec2 cursor = { context->spacing, context->spacing };
        for (size_t i = 0; i < numChildren; ++i)
        {
            cursor.x += itemSize.x + context->spacing;
            if (cursor.x + itemSize.x + context->spacing > thisRect.rect.size.x / window->GetContentScale())
            {
                cursor.x = context->spacing;
                cursor.y += itemSize.y + context->spacing;
            }
        }

        desiredSize.y = static_cast<int>((cursor.y + itemSize.y) * window->GetContentScale());
        return desiredSize;
    }

    template<>
    void Layout::LayoutWidgetChildren<components::GridBoxComponent>(ecs::World* world,
                                                                    ecs::System* system,
                                                                    const ecs::Id& entity,
                                                                    components::RectTransformComponent& gridBoxTransform,
                                                                    int layer,
                                                                    components::GridBoxComponent* gridBox)
    {
        // make sure our anchors are set correctly so that desired size works as expected.
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>();
        system->RunChildQuery(entity, dec,
                      [](const ecs::SystemUpdateData& updateData)
                      {
                          const auto& children = updateData.GetEntities();
                          auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                          for (size_t i = 0; i < children.size(); ++i)
                          {
                              auto& rect = rects[i];
                              rect.anchors = { 0.f, 1.f, 0.f, 0.f };
                              rect.minY = 0;
                              rect.maxY = 0;
                              rect.minX = 0;
                              rect.maxX = 0;
                          }

                          return false;
                      });

        auto itemSize = util::GetGridBoxItemSize(entity, system, gridBoxTransform);
        auto childRects = util::GetChildRectTransforms(entity, system);
        auto window = Application::Get()->GetWindow();

        math::IntVec2 cursor = { gridBox->spacing, gridBox->spacing };
        for (const auto& child: world->GetChildren(entity))
        {
            auto* rectTransform = childRects.at(child);

            rectTransform->anchors = { 0.f, 0.f, 0.f, 0.f };
            rectTransform->minX = static_cast<float>(cursor.x);
            rectTransform->minY = static_cast<float>(cursor.y);
            rectTransform->maxX = static_cast<float>(cursor.x + itemSize.x);
            rectTransform->maxY = static_cast<float>(cursor.y + itemSize.y);

            rectTransform->rect = util::CalculateScreenSpaceRect(*rectTransform,
                                                                 gridBoxTransform);
            rectTransform->layer = gridBoxTransform.layer;

            cursor.x += itemSize.x + gridBox->spacing;
            if (cursor.x + itemSize.x + gridBox->spacing > gridBoxTransform.rect.size.x / window->GetContentScale())
            {
                cursor.x = gridBox->spacing;
                cursor.y += itemSize.y + gridBox->spacing;
            }
        }

        gridBoxTransform.rect.size.y = static_cast<int>((cursor.y + itemSize.y) * window->GetContentScale());
        gridBoxTransform.maxY = gridBoxTransform.minY + cursor.y;

        dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>();
        util::ForEachWidgetChild(entity, system, dec,
         [world, system, layer](const ecs::SystemUpdateData& updateData,
                                                        auto&& widgetComp)
         {
             const auto& child = updateData.GetEntity();
             auto* rect = updateData.GetComponentArray<components::RectTransformComponent>();

             LayoutWidgetChildren(world,
                                      system,
                                      child,
                                      *rect,
                                      layer,
                                      widgetComp);
             rect->needsLayout = false;

             return false;
         });

        gridBoxTransform.needsLayout = false;
    }
}
