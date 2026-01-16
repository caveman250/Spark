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

    constexpr int s_Padding = 3;

    bool IsHoveringSlider(const input::InputComponent* inputComp,
                          const components::RectTransformComponent& splitViewTransform,
                          const components::SplitViewComponent& splitView)
    {
        float padding = s_Padding * Application::Get()->GetWindow()->GetContentScale();
        switch (splitView.dir)
        {
            case components::SplitViewDirection::Vertical:
            {
                float entity1Height = splitViewTransform.rect.size.y * splitView.sliderPos;
                float sliderTop = splitViewTransform.rect.topLeft.y + entity1Height - padding;
                return inputComp->mouseY > sliderTop &&
                       inputComp->mouseY < sliderTop + padding * 2;
            }
            case components::SplitViewDirection::Horizontal:
            {
                float entity1Width = splitViewTransform.rect.size.x * splitView.sliderPos;
                float sliderLeft = splitViewTransform.rect.topLeft.x + entity1Width - padding;
                return inputComp->mouseX > sliderLeft &&
                       inputComp->mouseX < sliderLeft + padding * 2;
            }
            default:
                SPARK_ASSERT(false);
                return false;
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

        ecs::util::ForEachEntity(this, updateData,
         [this, world, entities, inputComp, splitViews, transforms, inputComps](size_t i)
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
                 for (const auto& mouseEvent: input.mouseEvents)
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
         });
    }
}
