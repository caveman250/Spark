#include "ScrollViewUpdateSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "Widgets.generated.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ScrollViewUpdateSystem)

    void ScrollViewUpdateSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ScrollViewUpdateSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* scrollViews = updateData.GetComponentArray<ui::components::ScrollViewComponent>();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
        const auto* mouseComps = updateData.GetComponentArray<const ui::components::ReceivesMouseEventsComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& scrollView = scrollViews[i];
            auto& rectTransform = rectTransforms[i];
            const auto& mouseComp = mouseComps[i];

            if (rectTransform.needsLayout)
            {
                auto declaration = ecs::ChildQueryDeclaration()
                    .WithComponent<components::RectTransformComponent>()
                    .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
                RunChildQuery(entity,
                    declaration,
                    [this, world, &rectTransform](const ecs::SystemUpdateData& updateData)
                    {
                        std::visit([this, world, &updateData, &rectTransform](auto&& value)
                        {
                            const auto& children = updateData.GetEntities();
                            auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

                            for (size_t i = 0; i < children.size(); ++i)
                            {
                                const auto& child = children[i];
                                auto& childTransform = childTransforms[i];
                                auto oldTransform = childTransform;
                                childTransform.anchors = { 0, 1, 0, 0 };
                                auto desiredSize = DesiredSizeCalculator::GetDesiredSize(this, child, rectTransform, childTransform, &value[i]);
                                childTransform.maxY = childTransform.minY + desiredSize.y;
                                childTransform.rect = util::CalculateScreenSpaceRect(childTransform, rectTransform);

                                if (!childTransform.overridesChildSizes)
                                {
                                    util::LayoutChildren(world, this, child, rectTransform, childTransform.layer + 1);
                                    childTransform.needsLayout = false;
                                }
                                else
                                {
                                    childTransform.needsLayout = true;
                                }
                            }
                        }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES>());

                        return false;
                    });

                rectTransform.needsLayout = false;
            }

            if (mouseComp.hovered)
            {
                for (const auto& mouseEvent: mouseComp.mouseEvents)
                {
                    if (mouseEvent.scrollDelta != 0)
                    {
                        std::vector<std::pair<components::RectTransformComponent*, components::WidgetComponent*>>
                                childComponents;
                        int minChildY = INT_MAX;
                        int maxChildY = 0;
                        auto declaration = ecs::ChildQueryDeclaration()
                            .WithComponent<components::RectTransformComponent>()
                            .WithComponent<components::WidgetComponent>();
                        RunChildQuery(
                            entity,
                            declaration,
                            [&childComponents, &maxChildY, &minChildY](const ecs::SystemUpdateData& updateData)
                            {
                                const auto& children = updateData.GetEntities();
                                auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
                                auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

                                for (size_t i = 0; i < children.size(); ++i)
                                {
                                    auto& childTransform = childTransforms[i];
                                    minChildY = std::min(childTransform.rect.topLeft.y, minChildY);
                                    maxChildY = std::max(childTransform.rect.topLeft.y + childTransform.rect.size.y,
                                                         maxChildY);
                                    childComponents.emplace_back(std::make_pair(&childTransform, &widgets[i]));
                                }

                                return false;
                            });

                        int scrollBoxMaxY = rectTransform.rect.topLeft.y + rectTransform.rect.size.y;

                        int availableScrollSpaceTop = std::max(0, rectTransform.rect.topLeft.y - minChildY);
                        int availableScrollSpaceBottom = std::max(0, maxChildY - scrollBoxMaxY);

                        if (mouseEvent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                        {
                            int delta = std::min(mouseEvent.scrollDelta, availableScrollSpaceBottom);
                            for (auto& child: childComponents)
                            {
                                child.first->minY -= delta;
                                child.first->maxY -= delta;

                                child.first->rect = util::CalculateScreenSpaceRect(*child.first, rectTransform);
                                bool outOfBounds = (child.first->rect.topLeft.y + child.first->rect.size.y < rectTransform.rect.topLeft.y) ||
                                                   (child.first->rect.topLeft.y > rectTransform.rect.topLeft.y + rectTransform.rect.
                                                    size.y);
                                child.second->updateEnabled = !outOfBounds;
                                child.second->renderingEnabled = !outOfBounds;
                                child.first->needsLayout = true;
                            }
                            availableScrollSpaceTop += delta;
                            availableScrollSpaceBottom -= delta;
                            availableScrollSpaceTop = std::max(0, availableScrollSpaceTop);
                            debug::Log::Error("Scroll Top: {}", availableScrollSpaceTop);
                            scrollView.scrollAmount = static_cast<float>(availableScrollSpaceTop) /
                                                      static_cast<float>(availableScrollSpaceTop + availableScrollSpaceBottom);
                            scrollView.onScrolled.Broadcast(&rectTransform, scrollView.scrollAmount);
                        }
                        else if (mouseEvent.scrollDelta < 0 && availableScrollSpaceTop > 0)
                        {
                            int delta = std::min(-mouseEvent.scrollDelta, availableScrollSpaceTop);
                            for (auto& child: childComponents)
                            {
                                child.first->minY += delta;
                                child.first->maxY += delta;

                                child.first->rect = util::CalculateScreenSpaceRect(*child.first, rectTransform);
                                bool outOfBounds = (child.first->rect.topLeft.y + child.first->rect.size.y < rectTransform.rect.topLeft.y) ||
                                                   (child.first->rect.topLeft.y > rectTransform.rect.topLeft.y + rectTransform.rect.
                                                    size.y);
                                child.second->updateEnabled = !outOfBounds;
                                child.second->renderingEnabled = !outOfBounds;
                                child.first->needsLayout = true;
                            }

                            availableScrollSpaceTop -= delta;
                            availableScrollSpaceBottom += delta;
                            availableScrollSpaceTop = std::max(0, availableScrollSpaceTop);
                            debug::Log::Error("Scroll Top: {}", availableScrollSpaceTop);
                            scrollView.scrollAmount = static_cast<float>(availableScrollSpaceTop) /
                                                      static_cast<float>(availableScrollSpaceTop + availableScrollSpaceBottom);
                            scrollView.onScrolled.Broadcast(&rectTransform, scrollView.scrollAmount);
                        }
                    }
                }
            }
        }
    }
}
