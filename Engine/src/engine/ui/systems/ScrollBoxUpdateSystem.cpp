#include "ScrollBoxUpdateSystem.h"

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
    DEFINE_SPARK_SYSTEM(ScrollBoxUpdateSystem)

    void ScrollBoxUpdateSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                         components::ScrollBoxComponent* scrollBoxes,
                                         components::RectTransformComponent* rectTransforms,
                                         const components::ReceivesMouseEventsComponent* mouseComps)
    {
        PROFILE_SCOPE("ScrollBoxUpdateSystem::OnUpdate")

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& scrollBox = scrollBoxes[i];
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
                        RunChildQuery<components::RectTransformComponent,
                            components::WidgetComponent>(
                            entity,
                            [&childComponents, &maxChildY, &minChildY](const std::vector<ecs::Id>& children,
                                                                       components::RectTransformComponent*
                                                                       childTransforms,
                                                                       components::WidgetComponent* widgets)
                            {
                                for (size_t i = 0; i < children.size(); ++i)
                                {
                                    auto& childTransform = childTransforms[i];
                                    minChildY = std::min(childTransform.rect.topLeft.y, minChildY);
                                    maxChildY = std::max(childTransform.rect.topLeft.y + childTransform.rect.size.y,
                                                         maxChildY);
                                    childComponents.emplace_back(std::make_pair(&childTransform, &widgets[i]));
                                }

                                return true;
                            });

                        int scrollBoxMaxY = rectTransform.rect.topLeft.y + rectTransform.rect.size.y;

                        int availableScrollSpaceTop = rectTransform.rect.topLeft.y - minChildY;
                        int availableScrollSpaceBottom = maxChildY - scrollBoxMaxY;

                        if (mouseEvent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                        {
                            int delta = mouseEvent.scrollDelta * std::min(10, availableScrollSpaceBottom);
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
                            }
                            availableScrollSpaceBottom -= delta;
                            availableScrollSpaceTop += delta;
                            scrollBox.scrollAmount = 1.f - static_cast<float>(availableScrollSpaceBottom) /
                                                     (static_cast<float>(availableScrollSpaceBottom) + static_cast<
                                                          float>(availableScrollSpaceTop));
                            scrollBox.onScrolled.Broadcast(&rectTransform, scrollBox.scrollAmount);
                        } else if (mouseEvent.scrollDelta < 0 && availableScrollSpaceTop > 0)
                        {
                            int delta = mouseEvent.scrollDelta * std::min(10, availableScrollSpaceTop);
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
                            }

                            availableScrollSpaceTop += delta;
                            availableScrollSpaceBottom -= delta;
                            scrollBox.scrollAmount = 1.f - static_cast<float>(availableScrollSpaceBottom) /
                                                     (static_cast<float>(availableScrollSpaceBottom) + static_cast<
                                                          float>(availableScrollSpaceTop));
                            scrollBox.onScrolled.Broadcast(&rectTransform, scrollBox.scrollAmount);
                        }

                        rectTransform.needsLayout = true;
                    }
                }
            }
        }
    }
}
