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

                // Update facing angle
                // TODO make this a util method
                math::Vec3 worldPos = { transform.worldTransform[3].x, transform.worldTransform[3].y, transform.worldTransform[3].z };
                math::Vec3 cameraDir = math::Normalized(cameraComp->pos - worldPos);
                switch (gizmo.axis)
                {
                    case components::RotationAxis::X:
                        if (cameraDir.z < 0)
                        {
                            transform.rot.y = 90;
                        }
                        else
                        {
                            transform.rot.y = 270;
                        }
                        break;
                    case components::RotationAxis::Y:
                        if (cameraDir.x < 0)
                        {
                            transform.rot.z = 180;
                            transform.rot.x = 270;
                        }
                        else
                        {
                            transform.rot.z = 0;
                            transform.rot.x = 90;
                        }
                        if (cameraDir.z < 0)
                        {
                            transform.rot.x += 180;
                        }

                        break;
                    case components::RotationAxis::Z:
                        if (cameraDir.x < 0)
                        {
                            transform.rot.y = 180;
                        }
                        else
                        {
                            transform.rot.y = 0;
                        }
                        break;
                }

                math::Vec3 forward(cos(cameraComp->rot.x) * sin(cameraComp->rot.y),
                             sin(cameraComp->rot.x),
                             cos(cameraComp->rot.x) * cos(cameraComp->rot.y));
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
                    //project to circle
                    math::Vec2 initialHitPoint2;
                    math::Vec2 hitPoint2;
                    auto twelveoClock = math::Vec2(0.f,  1.f);
                    switch (gizmo.axis)
                    {
                        case components::RotationAxis::X:
                            initialHitPoint2 = math::Normalized(math::Vec2(gizmo.initialClickPos.y, gizmo.initialClickPos.z) - math::Vec2(worldPos.y, worldPos.z));
                            hitPoint2 = math::Normalized(math::Vec2(hitPoint.y, hitPoint.z) - math::Vec2(worldPos.y, worldPos.z));
                            break;
                        case components::RotationAxis::Y:
                            initialHitPoint2 = math::Normalized(math::Vec2(gizmo.initialClickPos.z, gizmo.initialClickPos.x) - math::Vec2(worldPos.z, worldPos.x));
                            hitPoint2 = math::Normalized(math::Vec2(hitPoint.z, hitPoint.x) - math::Vec2(worldPos.z, worldPos.x));
                            break;
                        case components::RotationAxis::Z:
                            initialHitPoint2 = math::Normalized(math::Vec2(gizmo.initialClickPos.x, gizmo.initialClickPos.y) - math::Vec2(worldPos.x, worldPos.y));
                            hitPoint2 = math::Normalized(math::Vec2(hitPoint.x, hitPoint.y) - math::Vec2(worldPos.x, worldPos.y));
                            break;
                    }
                    float angle = atan2(hitPoint2.y - twelveoClock.y, hitPoint2.x - twelveoClock.x);
                    float angle2 = atan2(initialHitPoint2.y - twelveoClock.y, initialHitPoint2.x - twelveoClock.x);
                    gizmo.onRotate.Broadcast((angle - angle2) * 2.f);
                }
            }
        });
    }
}
