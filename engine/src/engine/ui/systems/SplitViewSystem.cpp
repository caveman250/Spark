#include "spark.h"
#include "SplitViewSystem.h"

#include <Widgets.generated.h>

#include "RectTransformSystem.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include <easy/profiler.h>
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/math/IntVec2.h"
#include "platform/MouseCursorUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration SplitViewSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Collapsing Header System")
                    .WithComponent<components::SplitViewComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<const components::MouseInputComponent>()
                    .WithSingletonComponent<const input::InputComponent>()
                    .WithDependency<RectTransformSystem>();
    }

    constexpr int s_Padding = 6;

    bool IsHoveringSlider(const input::InputComponent* inputComp,
                            const components::RectTransformComponent& splitViewTransform,
                            const components::SplitViewComponent& splitView)
    {
        switch (splitView.dir)
        {
            case components::SplitViewDirection::Vertical:
            {
                float entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
                float sliderTop = splitViewTransform.rect.topLeft.y + entity1Height - s_Padding;
                return inputComp->mouseY > sliderTop &&
                    inputComp->mouseY < sliderTop + s_Padding * 2;
            }
            case components::SplitViewDirection::Horizontal:
            {
                float entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
                float sliderLeft = splitViewTransform.rect.topLeft.x + entity1Width - s_Padding;
                return inputComp->mouseX > sliderLeft &&
                    inputComp->mouseX < sliderLeft + s_Padding * 2;
            }
            default:
                SPARK_ASSERT(false);
                return false;
        }

    }

    void ArrangeVertically(const ecs::Id& child,
                            components::RectTransformComponent& rectTransform,
                            const components::RectTransformComponent& splitViewTransform,
                            const components::SplitViewComponent& splitView)
    {
        if (child == splitView.entity1)
        {
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft;
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = static_cast<int>(splitViewTransform.rect.size.y * splitView.sliderPos - s_Padding);
        }
        else if (child == splitView.sliderEntity)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, static_cast<int>(entity1Height - s_Padding));
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = s_Padding * 2;
        }
        else if (child == splitView.entity2)
        {
            auto entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(0, static_cast<int>(entity1Height + s_Padding));
            rectTransform.rect.size.x = splitViewTransform.rect.size.x;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y - static_cast<int>(entity1Height);
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
            rectTransform.rect.size.x = static_cast<int>(splitViewTransform.rect.size.x * splitView.sliderPos - s_Padding);
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView.sliderEntity)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(static_cast<int>(entity1Width - s_Padding), 0);
            rectTransform.rect.size.x = s_Padding * 2;
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
        else if (child == splitView.entity2)
        {
            auto entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
            rectTransform.rect.topLeft = splitViewTransform.rect.topLeft + math::IntVec2(static_cast<int>(entity1Width + s_Padding), 0);
            rectTransform.rect.size.x = static_cast<int>(splitViewTransform.rect.size.x - entity1Width);
            rectTransform.rect.size.y = splitViewTransform.rect.size.y;
        }
    }

    void SplitViewSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("SplitViewSystem::OnUpdate");

        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();
        auto* splitViews = updateData.GetComponentArray<components::SplitViewComponent>();
        auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();
        const auto* inputComps = updateData.GetComponentArray<const components::MouseInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& splitView = splitViews[i];
            auto& input = inputComps[i];
            auto& splitViewTransform = transforms[i];

            if (splitView.isResizing && inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] != input::KeyState::Down)
            {
                splitView.isResizing = false;
            }

            if (input.hovered)
            {
                for (const auto& mouseEvent : input.mouseEvents)
                {
                    if (mouseEvent.button == input::MouseButton::Left &&
                        mouseEvent.state == input::KeyState::Down)
                    {
                        splitView.isResizing = true;
                    }
                }

                if (splitView.isResizing)
                {
                    switch (splitView.dir)
                    {
                        case components::SplitViewDirection::Vertical:
                            splitView.sliderPos = static_cast<float>(inputComp->mouseY - splitViewTransform.rect.topLeft.y) / splitViewTransform.rect.size.y;
                            break;
                        case components::SplitViewDirection::Horizontal:
                            splitView.sliderPos = static_cast<float>(inputComp->mouseX - splitViewTransform.rect.topLeft.x) / splitViewTransform.rect.size.x;
                            break;
                        default:
                            SPARK_ASSERT(false);
                    }

                }
                else if (IsHoveringSlider(inputComp, splitViewTransform, splitView))
                {
                    if (!splitView.isShowingResizeCursor)
                    {
                        switch (splitView.dir)
                        {
                            case components::SplitViewDirection::Vertical:
                                MouseCursorUtil::SetMouseCursor(MouseCursor::ResizeUpDown);
                                break;
                            case components::SplitViewDirection::Horizontal:
                                MouseCursorUtil::SetMouseCursor(MouseCursor::ResizeLeftRight);
                                break;
                            default:
                                SPARK_ASSERT(false);
                        }

                        splitView.isShowingResizeCursor = true;
                    }
                }
                else if (splitView.isShowingResizeCursor)
                {
                    MouseCursorUtil::SetMouseCursor(MouseCursor::Arrow);
                    splitView.isShowingResizeCursor = false;
                }
            }

            if (!math::FloatEqual(splitView.lastSliderPos, splitView.sliderPos))
            {
                splitViewTransform.needsLayout = true;
                splitView.lastSliderPos = splitView.sliderPos;
            }

            if (splitViewTransform.needsLayout)
            {
                auto dec = ecs::HeirachyQueryDeclaration()
                    .WithComponent<components::RectTransformComponent>();

                RunChildQuery(entity, dec,
                [this, world, splitView, splitViewTransform](const ecs::SystemUpdateData &updateData)
                {
                    const auto &children = updateData.GetEntities();
                    auto *rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

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
