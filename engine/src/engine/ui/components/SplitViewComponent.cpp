#include "SplitViewComponent.h"
#include <Widgets.generated.h>
#include "engine/ui/util/WidgetUtil.h"
namespace se::ui
{
    template<>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::SplitViewComponent>(ecs::System* system,
                                                                                               const ecs::Id&,
                                                                                               components::RectTransformComponent& thisRect,
                                                                                               const components::SplitViewComponent* context)
    {
        math::Vec2 desiredSize = { };
        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent titleRectTransform = { };
        titleRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRectTransform.rect = util::CalculateScreenSpaceRect(titleRectTransform, thisRect);

        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent contentsRectTransform = { };
        contentsRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        contentsRectTransform.rect = util::CalculateScreenSpaceRect(contentsRectTransform, thisRect);

        auto entity1size = GetDesiredSize<ecs::NullComponentType>(system, context->entity1, nullptr, thisRect, titleRectTransform, nullptr);
        auto entity2size = GetDesiredSize<ecs::NullComponentType>(system, context->entity2, nullptr, thisRect, contentsRectTransform, nullptr);

        desiredSize = math::Vec2(static_cast<float>(std::max(entity1size.x, entity2size.x)), static_cast<float>(entity1size.y + entity2size.y) + 5.f);

        return desiredSize;
    }

    constexpr int s_Padding = 3;

    inline void ArrangeVertically(const ecs::Id& child,
                                  components::RectTransformComponent& rectTransform,
                                  const components::RectTransformComponent& splitViewTransform,
                                  const components::SplitViewComponent* splitView)
    {
        float padding = s_Padding * Application::Get()->GetWindow()->GetContentScale();
        if (child == splitView->entity1)
        {
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft;
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = static_cast<int>(splitViewTransform.rect.size.y * splitView->sliderPos - padding);
        }
        else if (child == splitView->sliderEntity)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView->sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, static_cast<int>(entity1Height - padding));
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = static_cast<int>(padding * 2);
        }
        else if (child == splitView->entity2)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView->sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, static_cast<int>(entity1Height + padding));
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = static_cast<int>(splitViewTransform.rect.size.y - entity1Height - padding);
        }
    }

    inline void ArrangeHorizontally(const ecs::Id& child,
                                    components::RectTransformComponent& rectTransform,
                                    const components::RectTransformComponent& splitViewTransform,
                                    const components::SplitViewComponent* splitView)
    {
        float padding = s_Padding * Application::Get()->GetWindow()->GetContentScale();
        if (child == splitView->entity1)
        {
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft;
            rectTransform.rect.size.x = static_cast<int>(splitViewTransform.rect.size.x * splitView->sliderPos - padding);
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView->sliderEntity)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView->sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(static_cast<int>(entity1Width - padding), 0);
            rectTransform.rect.size.x = static_cast<int>(padding * 2);
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView->entity2)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView->sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(static_cast<int>(entity1Width + padding), 0);
            rectTransform.rect.size.x = static_cast<int>(splitViewTransform.rect.size.x - entity1Width - padding);
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
    }

    template<>
    void Layout::LayoutWidgetChildren<components::SplitViewComponent>(ecs::World* world,
        ecs::System* system,
        const ecs::Id& entity,
        components::RectTransformComponent& splitViewTransform,
        int layer,
        components::SplitViewComponent* splitView)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>();

        util::ForEachWidgetChild(entity, system, dec,
                                 [system, world, splitView, splitViewTransform, layer](const ecs::SystemUpdateData& updateData, auto&& widgetComps)
                                 {
                                     const auto& children = updateData.GetEntities();
                                     auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

                                     for (size_t i = 0; i < children.size(); ++i)
                                     {
                                         const auto& child = children[i];
                                         auto& rectTransform = rectTransforms[i];

                                         switch (splitView->dir)
                                         {
                                             case components::SplitViewDirection::Vertical:
                                                 ArrangeVertically(child, rectTransform, splitViewTransform, splitView);
                                                 break;
                                             case components::SplitViewDirection::Horizontal:
                                                 ArrangeHorizontally(child, rectTransform, splitViewTransform, splitView);
                                                 break;
                                             default:
                                                 SPARK_ASSERT(false);
                                         }

                                         rectTransform.layer = splitViewTransform.layer;

                                         LayoutWidgetChildren(world, system, child, rectTransform, layer, widgetComps + i);
                                         rectTransform.needsLayout = false;
                                     }

                                     return false;
                                 });

        splitViewTransform.needsLayout = false;
    }
}
