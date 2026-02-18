#include "WindowSystem.h"
#include <easy/profiler.h>
#include "spark.h"
#include "UIMouseInputSystem.h"
#include "engine/Application.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/RectTransformUtil.h"
#include "platform/IWindow.h"
#include "platform/MouseCursorUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration WindowSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Window System")
                    .WithComponent<components::WindowComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithComponent<const components::MouseInputComponent>()
                    .WithSingletonComponent<const input::InputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void WindowSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("WindowSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* windows = updateData.GetComponentArray<components::WindowComponent>();
            auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();
            const auto* inputComps = updateData.GetComponentArray<const components::MouseInputComponent>();
            const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();
            const auto* appWindow = Application::Get()->GetWindow();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto& window = windows[i];
                auto& transform = transforms[i];
                auto input = inputComps[i];

                bool isMouseDown = false;
                for (const auto& mouseEvent : input.mouseEvents)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        isMouseDown = mouseEvent.state == input::KeyState::Down;
                        window.isResizing &= isMouseDown;
                    }
                }

                if (window.isResizing)
                {
                    window.isResizing &= inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down;
                }

                if (!window.isResizing && !window.docked)
                {
                    if (input.hovered)
                    {
                        const bool hoveringTop = std::abs(inputComp->mouseY - transform.rect.topLeft.y) < 5;
                        const bool hoveringBottom = std::abs(inputComp->mouseY - (transform.rect.topLeft.y + transform.rect.size.y)) < 5;
                        const bool hoveringLeft = std::abs(inputComp->mouseX - transform.rect.topLeft.x) < 5;
                        const bool hoveringRight =  std::abs(inputComp->mouseX - (transform.rect.topLeft.x + transform.rect.size.x)) < 5;

                        if (hoveringLeft || hoveringRight)
                        {
                            if (!window.isShowingResizeCursor)
                            {
                                MouseCursorUtil::SetMouseCursor(MouseCursor::ResizeLeftRight);
                                window.isShowingResizeCursor = true;
                            }

                            if (isMouseDown)
                            {
                                window.isResizing = true;
                                window.resizeDirection = hoveringLeft ?
                                    components::WindowResizeDirection::Left :
                                    components::WindowResizeDirection::Right;
                            }
                        }
                        else if (hoveringTop || hoveringBottom)
                        {
                            if (!window.isShowingResizeCursor)
                            {
                                MouseCursorUtil::SetMouseCursor(MouseCursor::ResizeUpDown);
                                window.isShowingResizeCursor = true;
                            }

                            if (isMouseDown)
                            {
                                window.isResizing = true;
                                window.resizeDirection = hoveringTop ?
                                    components::WindowResizeDirection::Up :
                                    components::WindowResizeDirection::Down;
                            }
                        }
                        else if (window.isShowingResizeCursor)
                        {
                            MouseCursorUtil::SetMouseCursor(MouseCursor::Arrow);
                            window.isShowingResizeCursor = false;
                        }
                    }
                    else if (!window.isResizing && window.isShowingResizeCursor)
                    {
                        MouseCursorUtil::SetMouseCursor(MouseCursor::Arrow);
                        window.isShowingResizeCursor = false;
                    }
                }

                if (window.isResizing)
                {
                    switch (window.resizeDirection)
                    {
                        case components::WindowResizeDirection::Up:
                            transform.minY += inputComp->mouseDeltaY / Application::Get()->GetWindow()->GetContentScale();
                            break;
                        case components::WindowResizeDirection::Down:
                            transform.maxY += inputComp->mouseDeltaY / Application::Get()->GetWindow()->GetContentScale();
                            break;
                        case components::WindowResizeDirection::Left:
                            transform.minX += inputComp->mouseDeltaX / Application::Get()->GetWindow()->GetContentScale();
                            break;
                        case components::WindowResizeDirection::Right:
                            transform.maxX += inputComp->mouseDeltaX / Application::Get()->GetWindow()->GetContentScale();
                            break;
                    }
                }

                if (window.pendingDeltaX != 0 || window.pendingDeltaY != 0)
                {
                    if (!window.isResizing && !window.docked)
                    {
                        const float invContentScale = 1.f / appWindow->GetContentScale();
                        transform.minX += window.pendingDeltaX * invContentScale;
                        transform.maxX += window.pendingDeltaX * invContentScale;
                        transform.minY += window.pendingDeltaY * invContentScale;
                        transform.maxY += window.pendingDeltaY * invContentScale;

                        math::Vec2 delta = math::Vec2(window.pendingDeltaX, window.pendingDeltaY);
                        transform.rect.topLeft += delta;
                        util::TranslateChildren(entities[i], this, delta);
                    }

                    window.pendingDeltaX = 0;
                    window.pendingDeltaY = 0;
                }
            }
        });
    }
}
