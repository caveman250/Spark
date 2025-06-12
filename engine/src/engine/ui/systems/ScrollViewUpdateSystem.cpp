#include "ScrollViewUpdateSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "Widgets.generated.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void ScrollViewUpdateSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ScrollViewUpdateSystem::OnUpdate")

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto window = app->GetPrimaryWindow();

        const auto& entities = updateData.GetEntities();
        auto* scrollViews = updateData.GetComponentArray<components::ScrollViewComponent>();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
        const auto* mouseComps = updateData.GetComponentArray<const components::MouseInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& scrollView = scrollViews[i];
            auto& rectTransform = rectTransforms[i];
            const auto& mouseComp = mouseComps[i];

            if (rectTransform.needsLayout)
            {
                auto declaration = ecs::HeirachyQueryDeclaration()
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<components::WidgetComponent>()
                    .WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
                float totalChildSize = 0.f;
                RunChildQuery(entity,
                    declaration,
                    [this, world, window, &rectTransform, &totalChildSize](const ecs::SystemUpdateData& updateData)
                    {
                        std::visit([this, world, window, &updateData, &rectTransform, &totalChildSize](auto&& value)
                        {
                            const auto& children = updateData.GetEntities();
                            auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
                            auto widgets = updateData.GetComponentArray<components::WidgetComponent>();
                            for (size_t i = 0; i < children.size(); ++i)
                            {
                                const auto& child = children[i];
                                auto& childTransform = childTransforms[i];
                                auto oldTransform = childTransform;
                                childTransform.anchors = { 0, 1, 0, 0 };
                                auto desiredSize = DesiredSizeCalculator::GetDesiredSize(this, child, &widgets[i], rectTransform, childTransform, &value[i]);
                                childTransform.maxY = static_cast<int>(childTransform.minY + desiredSize.y / window->GetContentScale());
                                childTransform.rect = util::CalculateScreenSpaceRect(childTransform, rectTransform);
                                totalChildSize += childTransform.rect.size.y;
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

                float availableScrollSpaceBottom = std::max(0.f, totalChildSize - rectTransform.rect.size.y);
                if (availableScrollSpaceBottom > 0.f)
                {
                    util::TranslateChildren(entity, this, math::IntVec2(0, -availableScrollSpaceBottom * scrollView.scrollAmount));
                    scrollView.onScrolled.Broadcast(&rectTransform, scrollView.scrollAmount);
                }

                rectTransform.needsLayout = false;
            }

            if (mouseComp.hovered)
            {
                for (const auto& mouseEvent: mouseComp.mouseEvents)
                {
                    if (mouseEvent.scrollDelta != 0)
                    {
                        int minChildY = INT_MAX;
                        int maxChildY = 0;
                        auto declaration = ecs::HeirachyQueryDeclaration()
                            .WithComponent<components::RectTransformComponent>();
                        RunChildQuery(
                            entity,
                            declaration,
                            [&maxChildY, &minChildY](const ecs::SystemUpdateData& updateData)
                            {
                                const auto& children = updateData.GetEntities();
                                auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

                                for (size_t i = 0; i < children.size(); ++i)
                                {
                                    auto& childTransform = childTransforms[i];
                                    minChildY = std::min(childTransform.rect.topLeft.y, minChildY);
                                    maxChildY = std::max(childTransform.rect.topLeft.y + childTransform.rect.size.y,
                                                         maxChildY);
                                }

                                return false;
                            });

                        int scrollBoxMaxY = rectTransform.rect.topLeft.y + rectTransform.rect.size.y;

                        int availableScrollSpaceTop = std::max(0, rectTransform.rect.topLeft.y - minChildY);
                        int availableScrollSpaceBottom = std::max(0, maxChildY - scrollBoxMaxY);

                        if (mouseEvent.scrollDelta > 0 && availableScrollSpaceBottom > 0)
                        {
                            int delta = std::min(mouseEvent.scrollDelta, availableScrollSpaceBottom);
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
                            int delta = std::min(-mouseEvent.scrollDelta, availableScrollSpaceTop);
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
    }
}
