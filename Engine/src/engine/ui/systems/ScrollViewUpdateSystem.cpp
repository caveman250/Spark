#include "ScrollViewUpdateSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "engine/math/Mat4.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"
#include "engine/render/RenderCommand.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ScrollViewUpdateSystem)

    void ScrollViewUpdateSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ScrollViewUpdateSystem::OnUpdate")

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

                        int availableScrollSpaceTop = rectTransform.rect.topLeft.y - minChildY;
                        int availableScrollSpaceBottom = maxChildY - scrollBoxMaxY;

                        if (mouseEvent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                        {
                            int delta = std::min(mouseEvent.scrollDelta, availableScrollSpaceBottom);
                            for (auto& child: childComponents)
                            {
                                child.first->minY -= delta;
                                child.first->maxY -= delta;

                                Rect rect = util::CalculateScreenSpaceRect(*child.first, rectTransform);
                                bool outOfBounds = (rect.topLeft.y + rect.size.y < rectTransform.rect.topLeft.y) ||
                                                   (rect.topLeft.y > rectTransform.rect.topLeft.y + rectTransform.rect.
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

                                Rect rect = util::CalculateScreenSpaceRect(*child.first, rectTransform);
                                bool outOfBounds = (rect.topLeft.y + rect.size.y < rectTransform.rect.topLeft.y) ||
                                                   (rect.topLeft.y > rectTransform.rect.topLeft.y + rectTransform.rect.
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

                        rectTransform.needsLayout = true;
                    }
                }
            }
        }
    }
}
