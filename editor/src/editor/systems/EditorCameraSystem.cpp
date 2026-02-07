#include "EditorCameraSystem.h"

#include <engine/math/math.h>

#include "GizmoSystem.h"
#include "editor/components/EditorCameraComponent.h"
#include "editor/util/ViewportUtil.h"
#include "engine/Application.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/util/SystemUtil.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
#include "engine/input/MouseButton.h"
#include "platform/IWindow.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration EditorCameraSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Editor Camera System")
            .WithComponent<components::EditorCameraComponent>()
            .WithComponent<ecs::components::TransformComponent>()
            .WithSingletonComponent<camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithDependency<GizmoSystem>();
    }

    void EditorCameraSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        auto editor = Application::Get()->GetEditorRuntime();
        if (editor->InGameMode())
        {
            return;
        }

        const auto& entities = updateData.GetEntities();
        auto* activeCamera = updateData.GetSingletonComponent<camera::ActiveCameraComponent>();
        auto* input = updateData.GetSingletonComponent<input::InputComponent>();
        auto* cameras = updateData.GetComponentArray<components::EditorCameraComponent>();
        auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
        
        if (entities.empty())
        {
            return;
        }

        auto app = Application::Get();
        SPARK_ASSERT(entities.size() == 1, "Multiple cameras not supported");

        auto dx = static_cast<float>(activeCamera->lastMouseX - input->mouseX);
        auto dy = static_cast<float>(activeCamera->lastMouseY - input->mouseY);
        activeCamera->lastMouseX = input->mouseX;
        activeCamera->lastMouseY = input->mouseY;

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& camera = cameras[i];
            auto& transform = transforms[i];

            if (util::PosWithinViewport(input->mouseX, input->mouseY))
            {
                input::InputUtil::ProcessMouseEvents(entity, input, [&camera](const input::MouseEvent& mouseEvent)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        if (mouseEvent.state == input::KeyState::Down)
                        {
                            camera.mouseDown = true;
                        }
                        else
                        {
                            camera.mouseDown = false;
                        }

                        return true;
                    }

                    return false;
                });
            }

            if (camera.mouseDown)
            {
                auto dt = app->GetDeltaTime();
                transform.rot = transform.rot + math::Vec3(dy * dt, dx * dt, 0.0f);
            }

            math::Vec3 forward(cos(transform.rot.x) * sin(transform.rot.y),
                         sin(transform.rot.x),
                         cos(transform.rot.x) * cos(transform.rot.y));

            math::Vec3 right = math::Vec3(
                sin(transform.rot.y - 3.14f / 2.0f),
                0,
                cos(transform.rot.y - 3.14f / 2.0f));

            math::Vec3 up = math::Cross(right, forward);

            if (util::PosWithinViewport(input->mouseX, input->mouseY) &&
                !input::InputUtil::IsAnyModifierKeyDown(input))
            {
                // Movement
                if (input->keyStates.at(static_cast<int>(input::Key::W)) == input::KeyState::Down)
                {
                    transform.pos += forward * 5.f * Application::Get()->GetDeltaTime();
                }

                if (input->keyStates.at(static_cast<int>(input::Key::S)) == input::KeyState::Down)
                {
                    transform.pos -= forward * 5.f * Application::Get()->GetDeltaTime();
                }

                if (input->keyStates.at(static_cast<int>(input::Key::A)) == input::KeyState::Down)
                {
                    transform.pos -= right * 5.f * Application::Get()->GetDeltaTime();
                }

                if (input->keyStates.at(static_cast<int>(input::Key::D)) == input::KeyState::Down)
                {
                    transform.pos += right * 5.f * Application::Get()->GetDeltaTime();
                }
            }

            activeCamera->pos = transform.pos;
            activeCamera->rot = transform.rot;
            activeCamera->view = math::LookAt(
                    transform.pos,
                    transform.pos + forward,
                    up);

            auto gameViewport = app->GetGameViewportSize();
            activeCamera->proj = math::Perspective(math::Radians(45.f), (float)gameViewport.x / (float)gameViewport.y,.1f, 100.f);
        }
    }
}
