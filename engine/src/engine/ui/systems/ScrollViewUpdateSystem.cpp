#include "ScrollViewUpdateSystem.h"

#include "RectTransformSystem.h"
#include "Widgets.generated.h"
#include "easy/profiler.h"
#include "engine/Application.h"
#include "engine/ui/systems/UIMouseInputSystem.h"
#include "engine/ui/util/RectTransformUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ScrollViewUpdateSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("ScrollViewUpdateSystem")
                    .WithComponent<components::ScrollViewComponent>()
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<const components::MouseInputComponent>()
                    .WithDependency<UIMouseInputSystem>()
                    .WithDependency<RectTransformSystem>()
                    .WithHeirachyQuery<components::WidgetComponent>();
    }

    void ScrollViewUpdateSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("ScrollViewUpdateSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* scrollViews = updateData.GetComponentArray<components::ScrollViewComponent>();
            const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
            const auto* mouseComps = updateData.GetComponentArray<const components::MouseInputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& scrollView = scrollViews[i];
                const auto& rectTransform = rectTransforms[i];
                const auto& mouseComp = mouseComps[i];

                if (mouseComp.hovered)
                {
                    SPARK_ASSERT(mouseComp.mouseEvents.size() < 5);
                    for (const auto& mouseEvent: mouseComp.mouseEvents)
                    {
                        if (mouseEvent.scrollDelta != 0)
                        {
                            int minChildY = INT_MAX;
                            int maxChildY = 0;
                            auto declaration = ecs::HeirachyQueryDeclaration()
                                .WithComponent<const components::RectTransformComponent>();
                            RunChildQuery(
                                entity,
                                declaration,
                                [&maxChildY, &minChildY](const ecs::SystemUpdateData& updateData)
                                {
                                    const auto* childTransform = updateData.GetComponentArray<const components::RectTransformComponent>();

                                    minChildY = std::min(childTransform->rect.topLeft.y, minChildY);
                                    maxChildY = std::max(childTransform->rect.topLeft.y + childTransform->rect.size.y,
                                                        maxChildY);

                                    return false;
                                });

                            const int scrollBoxMaxY = rectTransform.rect.topLeft.y + rectTransform.rect.size.y;

                            int availableScrollSpaceTop = std::max(0, rectTransform.rect.topLeft.y - minChildY);
                            int availableScrollSpaceBottom = std::max(0, maxChildY - scrollBoxMaxY);

                            if (mouseEvent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                            {
                                const int delta = std::min(mouseEvent.scrollDelta, availableScrollSpaceBottom);
                                util::TranslateChildren(entity, this, math::IntVec2(0, -delta));
                                availableScrollSpaceTop += delta;
                                availableScrollSpaceBottom -= delta;
                                availableScrollSpaceTop = std::max(0, availableScrollSpaceTop);
                                scrollView.scrollAmount = static_cast<float>(availableScrollSpaceTop) /
                                                          static_cast<float>(availableScrollSpaceTop + availableScrollSpaceBottom);
                                scrollView.onScrolled.Broadcast(&rectTransform, scrollView.scrollAmount);
                            }
                            else if (mouseEvent.scrollDelta < 0 && availableScrollSpaceTop > 0)
                            {
                                const int delta = std::min(-mouseEvent.scrollDelta, availableScrollSpaceTop);
                                util::TranslateChildren(entity, this, math::IntVec2(0, delta));
                                availableScrollSpaceTop -= delta;
                                availableScrollSpaceBottom += delta;
                                availableScrollSpaceTop = std::max(0, availableScrollSpaceTop);
                                scrollView.scrollAmount = static_cast<float>(availableScrollSpaceTop) /
                                                          static_cast<float>(availableScrollSpaceTop + availableScrollSpaceBottom);
                                scrollView.onScrolled.Broadcast(&rectTransform, scrollView.scrollAmount);
                            }
                        }
                    }
                }
            }
        });
    }
}
