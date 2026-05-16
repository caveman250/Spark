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
            auto* gizmos = updateData.GetComponentArray<components::RotationGizmoComponent>();
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

                // TODO make this a util method
                math::Vec3 worldPos = { transform.worldTransform[3].x, transform.worldTransform[3].y, transform.worldTransform[3].z };

                geo::Plane plane =
                {
                    .normal = { 0.f, 0.f, 1.f },
                    .center = worldPos
                };

                // Update facing angle
                mesh.materialInstance->SetUniform("world_pos", 1, &worldPos);
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
                        plane.normal = { 1.f, 0.f, 0.f };
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
                        plane.normal = { 0.f, 1.f, 0.f };
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
                        plane.normal = { 0.f, 0.f, 1.f };
                        break;
                }

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
                                auto planeHit = geo::util::RayCastPlane(ray, plane);
                                if (planeHit.has_value())
                                {
                                    gizmo.mouseDown = true;
                                    mesh.vertexBuffer = gizmo.fullVertBuffer;
                                    mesh.indexBuffer = gizmo.fullIndexBuffer;
                                    mesh.aabb = gizmo.fullAABB;
                                    gizmo.initialClickPos = planeHit.value().intersectionPoint;
                                    mesh.materialInstance->SetUniform("uniform_color", 1, &gizmo.selectedColor);
                                    gizmo.onBeginRotate.Broadcast();
                                    return true;
                                }
                            }
                        }

                        return false;
                    });
                }

                if (!gizmo.mouseDown && gizmo.wasMouseDown)
                {
                    gizmo.onFinishRotate.Broadcast();
                    gizmo.wasMouseDown = false;
                    mesh.vertexBuffer = gizmo.quarterVertBuffer;
                    mesh.indexBuffer = gizmo.quarterIndexBuffer;
                    mesh.aabb = gizmo.quarterAABB;
                    math::Vec3 gizmoData = {0.f, 1.f, 0.f };
                    mesh.materialInstance->SetUniform("gizmo_data", 1, &gizmoData);
                    gizmo.hasSetVisualisationDir = false;

                    auto yellow = math::Vec4(1.f, 1.f, 0.f, 1.f);
                    mesh.materialInstance->SetUniform("uniform_color", 1, gizmo.wasHovered ? &yellow : &gizmo.color);
                }
                else if (!hit.has_value() && gizmo.wasHovered && !gizmo.mouseDown)
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
                    if (!gizmo.hasSetVisualisationDir && std::abs(angle - angle2) > math::Radians(2))
                    {
                        if (angle < angle2)
                        {
                            gizmo.visualisationDir = 1.f;
                        }
                        else
                        {
                            gizmo.visualisationDir = 0.f;
                        }
                        gizmo.hasSetVisualisationDir = true;
                    }

                    if (gizmo.hasSetVisualisationDir)
                    {
                        math::Vec3 gizmoData = { angle, angle2, gizmo.visualisationDir };
                        if (gizmoData.x < 0)
                        {
                            gizmoData.x += M_PI;
                        }
                        if (gizmoData.y < 0)
                        {
                            gizmoData.y += M_PI;
                        }
                        gizmoData.x /= M_PI;
                        gizmoData.y /= M_PI;
                        mesh.materialInstance->SetUniform("gizmo_data", 1, &gizmoData);
                    }

                    angle = (angle - angle2);
                    gizmo.onRotate.Broadcast(angle * 2.f);
                }
            }
        });
    }
}
