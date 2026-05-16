#include "TranslationGizmoSystem.h"

#include "editor/components/TransformGizmoComponent.h"
#include "editor/util/ViewportUtil.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/geo/Plane.h"
#include "engine/geo/Ray.h"
#include "engine/geo/util/CollisionUtil.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration TranslationGizmoSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<components::TransformGizmoComponent>()
            .WithComponent<ecs::components::TransformComponent>()
            .WithComponent<ecs::components::MeshComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>();
    }

    void TranslationGizmoSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto editor = Application::Get()->GetEditor();
        if (editor->InGameMode())
        {
            return;
        }

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto cameraComp = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
            auto inputComp = updateData.GetSingletonComponent<input::InputComponent>();
            auto* gizmos = updateData.GetComponentArray<components::TransformGizmoComponent>();
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* meshes = updateData.GetComponentArray<ecs::components::MeshComponent>();

            auto ray = util::GetEditorMouseRay(inputComp, cameraComp);

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
                    math::Vec4 yellow = math::Vec4(1.f, 1.f, 0.f, 1.f);
                    mesh.materialInstance->SetUniform("uniform_color", 1, &yellow);

                    input::InputUtil::ProcessMouseEvents(entity, inputComp, [&gizmo](const input::MouseEvent& mouseEvent)
                    {
                        if (mouseEvent.button == input::MouseButton::Left)
                        {
                            if (mouseEvent.state == input::KeyState::Down)
                            {
                                gizmo.mouseDown = true;
                                return true;
                            }
                        }

                        return false;
                    });
                }
                else if (!gizmo.mouseDown)
                {
                    mesh.materialInstance->SetUniform("uniform_color", 1, &gizmo.color);
                }

                if (gizmo.mouseDown)
                {
                    math::Vec3 forward(cos(cameraComp->rot.x) * sin(cameraComp->rot.y),
                             sin(cameraComp->rot.x),
                             cos(cameraComp->rot.x) * cos(cameraComp->rot.y));
                    math::Vec3 worldPos = { transform.worldTransform[3].x, transform.worldTransform[3].y, transform.worldTransform[3].z };
                    geo::Plane plane = {
                        .normal = forward,
                        .center = worldPos
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
        });
    }
}
