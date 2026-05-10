#include "RotationGizmoSystem.h"

#include "editor/components/RotationGizmoComponent.h"
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
    ecs::SystemDeclaration RotationGizmoSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<components::RotationGizmoComponent>()
            .WithComponent<ecs::components::TransformComponent>()
            .WithComponent<ecs::components::MeshComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>();
    }

    void RotationGizmoSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto editor = Application::Get()->GetEditorRuntime();
        if (editor->InGameMode())
        {
            return;
        }

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [editor](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto cameraComp = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
            auto inputComp = updateData.GetSingletonComponent<input::InputComponent>();
            auto* gizmos = updateData.GetComponentArray<components::RotationGizmoComponent>();
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* meshes = updateData.GetComponentArray<ecs::components::MeshComponent>();

            auto mousePos = util::ScreenSpaceToGameViewportSpace(inputComp->mouseX, inputComp->mouseY);
            auto viewportRect = editor->GetViewportRect();
            math::Vec3 mouseWorldPos = util::ScreenToWorldPoint(mousePos
                                                                , cameraComp->view
                                                                , cameraComp->proj
                                                                , math::Vec4(0.f,
                                                                    0.f,
                                                                    static_cast<float>(viewportRect.size.x),
                                                                    static_cast<float>(viewportRect.size.y)));
            math::Vec3 direction = math::Normalized(mouseWorldPos - cameraComp->pos);
            auto ray = geo::Ray(cameraComp->pos, direction);

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

                math::Vec3 forward(cos(cameraComp->rot.x) * sin(cameraComp->rot.y),
                             sin(cameraComp->rot.x),
                             cos(cameraComp->rot.x) * cos(cameraComp->rot.y));
                math::Vec3 worldPos = { transform.worldTransform[3].x, transform.worldTransform[3].y, transform.worldTransform[3].z };
                geo::Plane plane =
                {
                    .normal = forward,
                    .distSquared = math::Dot(forward, worldPos)
                };

                gizmo.mouseDown &= inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down;

                auto hit = geo::util::RayCastAABB(ray, transform);
                if (hit.has_value())
                {
                    gizmo.wasHovered = true;

                    if (!gizmo.mouseDown)
                    {
                        auto yellow = math::Vec4(1.f, 1.f, 0.f, 1.f);
                        mesh.materialInstance->SetUniform("uniform_color", 1, &yellow);
                    }

                    input::InputUtil::ProcessMouseEvents(entity, inputComp, [&gizmo, &mesh, ray, plane](const input::MouseEvent& mouseEvent)
                    {
                        if (mouseEvent.button == input::MouseButton::Left)
                        {
                            if (mouseEvent.state == input::KeyState::Down)
                            {
                                gizmo.mouseDown = true;
                                mesh.vertexBuffer = gizmo.fullVertBuffer;
                                mesh.indexBuffer = gizmo.fullIndexBuffer;
                                mesh.aabb = gizmo.fullAABB;
                                gizmo.initialClickPos = geo::util::RayCastPlane(ray, plane).value().intersectionPoint;
                                mesh.materialInstance->SetUniform("uniform_color", 1, &gizmo.color);
                                gizmo.onBeginRotate.Broadcast();
                                return true;
                            }
                        }

                        return false;
                    });
                }
                else if (!gizmo.mouseDown && gizmo.wasMouseDown)
                {
                    gizmo.onFinishRotate.Broadcast();
                    gizmo.wasMouseDown = false;
                    mesh.vertexBuffer = gizmo.quarterVertBuffer;
                    mesh.indexBuffer = gizmo.quarterIndexBuffer;
                    mesh.aabb = gizmo.quarterAABB;
                }
                else if (gizmo.wasHovered)
                {
                    mesh.materialInstance->SetUniform("uniform_color", 1, &gizmo.color);
                    gizmo.wasHovered = false;
                }

                if (gizmo.mouseDown)
                {
                    gizmo.wasMouseDown = true;
                    auto hit = geo::util::RayCastPlane(ray, plane);
                    SPARK_ASSERT(hit.has_value());
                    math::Vec3 hitPoint = hit.value().intersectionPoint;
                    float dist = math::MagnitudeSquared(hitPoint - gizmo.initialClickPos);
                    dist = sqrtf(dist);
                    switch (gizmo.axis)
                    {
                        case components::RotationAxis::Z:
                        {
                            gizmo.onRotate.Broadcast(dist);
                            break;
                        }
                        case components::RotationAxis::X:
                        {
                            gizmo.onRotate.Broadcast(dist);
                            break;
                        }
                        case components::RotationAxis::Y:
                        {
                            gizmo.onRotate.Broadcast(dist);
                            break;
                        }
                    }
                }
            }
        });
    }
}
