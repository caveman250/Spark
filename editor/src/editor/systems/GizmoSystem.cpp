#include "GizmoSystem.h"

#include "editor/components/GizmoComponent.h"
#include "editor/util/ViewportUtil.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/geo/AABB.h"
#include "engine/geo/Plane.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
#include "platform/IWindow.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/geo/Ray.h"
#include "engine/geo/util/CollisionUtil.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration GizmoSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<components::GizmoComponent>()
            .WithComponent<ecs::components::TransformComponent>()
            .WithComponent<ecs::components::MeshComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>();
    }

    math::Vec3 ScreenToWorldPoint(const math::Vec2& screenPos, const math::Mat4& view,
             const math::Mat4& projection, const math::Vec4& viewport)
    {
        math::Vec3 win(screenPos.x, viewport.w - screenPos.y, 0.0f);
        math::Vec3 point = math::UnProject(win, view, projection, viewport);
        return point;
    }

    void GizmoSystem::OnUpdate(const ecs::SystemUpdateData& systemUpdateData)
    {
        auto editor = Application::Get()->GetEditorRuntime();
        const auto& entities = systemUpdateData.GetEntities();
        auto cameraComp = systemUpdateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
        auto inputComp = systemUpdateData.GetSingletonComponent<input::InputComponent>();
        auto* gizmos = systemUpdateData.GetComponentArray<components::GizmoComponent>();
        auto* transforms = systemUpdateData.GetComponentArray<ecs::components::TransformComponent>();
        auto* meshes = systemUpdateData.GetComponentArray<ecs::components::MeshComponent>();

        auto mousePos = util::ScreenSpaceToGameViewportSpace(inputComp->mouseX, inputComp->mouseY);
        auto viewportRect = editor->GetViewportRect();
        math::Vec3 mouseWorldPos = ScreenToWorldPoint(mousePos
           , cameraComp->view
           , cameraComp->proj
           , math::Vec4(0, 0, viewportRect.size.x, viewportRect.size.y));
        math::Vec3 direction = math::Normalized(mouseWorldPos - cameraComp->pos);
        geo::Ray ray = geo::Ray(cameraComp->pos, direction);

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& transform = transforms[i];
            auto& gizmo = gizmos[i];
            auto& mesh = meshes[i];

            // uninitialized
            if (!mesh.materialInstance)
            {
                continue;
            }

            gizmo.mouseDown &= inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down;

            if (geo::util::RayCastAABB(ray, transform))
            {
                math::Vec3 yellow = math::Vec3(1.f, 1.f, 0.f);
                mesh.materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &yellow);

                input::InputUtil::ProcessMouseEvents(entity, inputComp, [&gizmo](const input::MouseEvent& mouseEvent)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        if (mouseEvent.state == input::KeyState::Down)
                        {
                            gizmo.mouseDown = true;
                        }

                        return true;
                    }

                    return false;
                });
            }
            else if (!gizmo.mouseDown)
            {
                mesh.materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &gizmo.color);
            }

            if (gizmo.mouseDown)
            {
                math::Vec3 forward(cos(cameraComp->rot.x) * sin(cameraComp->rot.y),
                         sin(cameraComp->rot.x),
                         cos(cameraComp->rot.x) * cos(cameraComp->rot.y));
                math::Vec3 worldPos = { transform.worldTransform[3].x, transform.worldTransform[3].y, transform.worldTransform[3].z };
                geo::Plane plane = {
                    .normal = forward,
                    .distSquared = math::Dot(forward, worldPos)
                };
                auto hit = geo::util::RayCastPlane(ray, plane);
                SPARK_ASSERT(hit.has_value());
                switch (gizmo.axis)
                {
                    case components::GizmoAxis::Forward:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(0.f, 0.f, hit.value().intersectionPoint.z));
                        break;
                    }
                    case components::GizmoAxis::Right:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(hit.value().intersectionPoint.x, 0.f, 0.f));
                        break;
                    }
                    case components::GizmoAxis::Up:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(0.f, hit.value().intersectionPoint.y, 0.f));
                        break;
                    }
                }
            }
        }
    }
}
