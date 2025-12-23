#include "spark.h"
#include "SplitViewSystem.h"

#include <Widgets.generated.h>

#include "RectTransformSystem.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/math/IntVec2.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration SplitViewSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Collapsing Header System")
                    .WithComponent<components::SplitViewComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithHeirachyQuery<components::WidgetComponent>()
                    .WithDependency<RectTransformSystem>();
    }

    constexpr int s_Padding = 4;

    void ArrangeVertically(const ecs::Id& child,
                            components::RectTransformComponent& rectTransform,
                            const components::RectTransformComponent& splitViewTransform,
                            const components::SplitViewComponent& splitView)
    {
        if (child == splitView.entity1)
        {
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft;
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y * splitView.sliderPos - s_Padding;
        }
        else if (child == splitView.sliderEntity)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, entity1Height - s_Padding);
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = s_Padding * 2;
        }
        else if (child == splitView.entity2)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, entity1Height + s_Padding);
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y - entity1Height;
        }
    }

    void ArrangeHorizontally(const ecs::Id& child,
                            components::RectTransformComponent& rectTransform,
                            const components::RectTransformComponent& splitViewTransform,
                            const components::SplitViewComponent& splitView)
    {
        if (child == splitView.entity1)
        {
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft;
            rectTransform.rect.size.x = splitViewTransform.rect.size.x * splitView.sliderPos - s_Padding;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView.sliderEntity)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(entity1Width - s_Padding, 0);
            rectTransform.rect.size.x = s_Padding * 2;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView.entity2)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(entity1Width + s_Padding, 0);
            rectTransform.rect.size.x = splitViewTransform.rect.size.x - entity1Width;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
    }

    void SplitViewSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("SplitViewSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* splitViews = updateData.GetComponentArray<components::SplitViewComponent>();
        auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& splitView = splitViews[i];
            auto& splitViewTransform = transforms[i];

            if (!math::FloatEqual(splitView.lastSliderPos, splitView.sliderPos))
            {
                splitViewTransform.needsLayout = true;
                splitView.lastSliderPos = splitView.sliderPos;
                continue;
            }

            if (splitViewTransform.needsLayout)
            {
                auto dec = ecs::HeirachyQueryDeclaration()
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<const components::WidgetComponent>();

                RunChildQuery(entity, dec,
                [this, world, splitView, splitViewTransform](const ecs::SystemUpdateData &updateData)
                {
                    const auto &children = updateData.GetEntities();
                    auto *rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
                    auto *widgets = updateData.GetComponentArray<const components::WidgetComponent>();

                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        const auto &child = children[i];
                        auto &rectTransform = rectTransforms[i];

                        switch (splitView.dir)
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


                        if (!rectTransform.overridesChildSizes)
                        {
                            util::LayoutChildren(world, this, child, rectTransform, rectTransform.layer);
                            rectTransform.needsLayout = false;
                        }
                        else
                        {
                            rectTransform.needsLayout = true;
                        }
                    }

                    return false;
                });

                splitViewTransform.needsLayout = false;
            }
        }
    }
}
