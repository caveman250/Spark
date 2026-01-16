#include "CollapsingHeaderComponent.h"

#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    template <>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::CollapsingHeaderComponent>(ecs::System* system,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::CollapsingHeaderComponent* context)
    {
        math::Vec2 desiredSize = {};
        // make a fake rect transform for the title since we cant access the real component here
        components::RectTransformComponent titleRectTransform = {};
        titleRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRectTransform.rect = util::CalculateScreenSpaceRect(titleRectTransform, thisRect);
        if (context->collapsed)
        {
            desiredSize = GetDesiredSize<ecs::NullComponentType>(system, context->titleEntity, nullptr, thisRect, titleRectTransform, nullptr);
        }
        else
        {
            // make a fake rect transform for the title since we cant access the real component here
            components::RectTransformComponent contentsRectTransform = {};
            contentsRectTransform.anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentsRectTransform.rect = util::CalculateScreenSpaceRect(contentsRectTransform, thisRect);

            auto headerSize = GetDesiredSize<ecs::NullComponentType>(system, context->titleEntity, nullptr, thisRect, titleRectTransform, nullptr);
            auto contentSize = GetDesiredSize<ecs::NullComponentType>(system, context->contentsEntity, nullptr, thisRect, contentsRectTransform, nullptr);

            desiredSize = math::Vec2(static_cast<float>(std::max(headerSize.x, contentSize.x)), static_cast<float>(headerSize.y + contentSize.y) + 5.f);
        }

        return desiredSize;
    }

    template <>
    void Layout::LayoutWidgetChildren<components::CollapsingHeaderComponent>(ecs::World* world,
                                 ecs::System* system,
                                 const ecs::Id& entity,
                                 components::RectTransformComponent& collapsingHeaderRect,
                                 int layer,
                                 components::CollapsingHeaderComponent* collapsingHeader)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<components::WidgetComponent>();
        std::unordered_map<ecs::Id, util::ChildDesiredSizeInfo> childRects = { };

        util::ForEachWidgetChild(entity, system, dec,
         [system, collapsingHeaderRect, &childRects](const ecs::SystemUpdateData& updateData,
                                                        auto&& widgetComp)
         {
             const auto& child = updateData.GetEntity();
             auto* rect = updateData.GetComponentArray<components::RectTransformComponent>();
             auto* widget = updateData.GetComponentArray<components::WidgetComponent>();

             rect->anchors = { 0.f, 1.f, 0.f, 1.f };
             rect->minY = 0;
             rect->maxY = 0;
             rect->minX = 0;
             rect->maxX = 0;

             math::IntVec2 childDesiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, widget, collapsingHeaderRect, *rect, widgetComp);
             childRects[child] = util::ChildDesiredSizeInfo(rect, childDesiredSize);

             return false;
         });

        for (const auto& child: world->GetChildren(entity))
        {
            auto desiredSizeInfo = childRects.at(child);

            if (child == collapsingHeader->contentsEntity && collapsingHeader->collapsed)
            {
                continue;
            }

            if (child == collapsingHeader->titleEntity)
            {
                desiredSizeInfo.rectTransform->rect.topLeft = collapsingHeaderRect.rect.topLeft;
                desiredSizeInfo.rectTransform->rect.size.x = collapsingHeaderRect.rect.size.x;
                desiredSizeInfo.rectTransform->rect.size.y = desiredSizeInfo.desiredSize.y;
            }
            else if (child == collapsingHeader->contentsEntity)
            {
                const auto& titleDesired = childRects[collapsingHeader->titleEntity];
                int titleMaxY = titleDesired.desiredSize.y;

                desiredSizeInfo.rectTransform->rect.topLeft = collapsingHeaderRect.rect.topLeft + math::Vec2(0.f, titleMaxY + 5.f);
                desiredSizeInfo.rectTransform->rect.size.x = collapsingHeaderRect.rect.size.x;
                desiredSizeInfo.rectTransform->rect.size.y = desiredSizeInfo.desiredSize.y;
            }
            else
            {
                SPARK_ASSERT(false, "Unexpected CollapsingHeader child.");
            }
        }

        util::ForEachWidgetChild(entity, system, dec, [world, layer, system](const ecs::SystemUpdateData& updateData, auto&& widgetComp)
        {
            const auto& entity = updateData.GetEntity();
            auto* rectTransform = updateData.GetComponentArray<components::RectTransformComponent>();

            LayoutWidgetChildren(world, system, entity, *rectTransform, layer, widgetComp);
            rectTransform->needsLayout = false;
        });

        collapsingHeaderRect.needsLayout = false;
    }
}
