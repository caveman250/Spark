#include "GizmoSystem.h"

#include "editor/components/GizmoComponent.h"
#include "editor/util/ViewportUtil.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
#include "platform/IWindow.h"
#include "engine/ui/components/RectTransformComponent.h"

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

    struct Ray
    {
        math::Vec3 origin;
        math::Vec3 direction;
    };

    struct HitInfo
    {
        bool hit = false;
        math::Vec3 hitPoint = {};
        float distance = std::numeric_limits<float>::max();
    };

    struct RaycastHit
    {
        HitInfo hitInfo = {};
        ecs::Id entity = ecs::s_InvalidEntity;
    };

    struct AABB
    {
        math::Vec3 min;
        math::Vec3 size;
    };


    bool RayAABoxIntersection(const Ray& ray, const AABB& box, HitInfo& outHitInfo)
    {
        math::Vec3 tMin = (box.min - ray.origin) / ray.direction;
        math::Vec3 tMax = (box.min + box.size - ray.origin) / ray.direction;

        // Find the nearest and farthest intersections
        math::Vec3 tNear = { std::min(tMin.x, tMax.x), std::min(tMin.y, tMax.y), std::min(tMin.z, tMax.z)};
        math::Vec3 tFar = { std::max(tMin.x, tMax.x), std::max(tMin.y, tMax.y), std::max(tMin.z, tMax.z)};

        // Find the maximum of the nearest intersections
        float tNearMax = std::max(std::max(tNear.x, tNear.y), tNear.z);
        // Find the minimum of the farthest intersections
        float tFarMin = std::min(std::min(tFar.x, tFar.y), tFar.z);

        // Check if the ray intersects the bounding box
        if (tNearMax <= tFarMin)
        {
            // Ray intersects the bounding box
            // Calculate the intersection point
            math::Vec3 intersectionPoint = ray.origin + ray.direction * tNearMax;
            outHitInfo.hitPoint = intersectionPoint;
            outHitInfo.distance = tNearMax;
            outHitInfo.hit = true;

            return true;
        }

        // Ray does not intersect the bounding box
        return false;
    }

    bool IntersectRayBoundingBox(const Ray& ray, RaycastHit& outRaycastHit, const ecs::components::TransformComponent& transform, const AABB& aabb)
    {
        // Transform the ray to the local space of the entity
        math::Vec4 origin4 = math::Inverse(transform.worldTransform) * math::Vec4(ray.origin, 1.0f);
        math::Vec3 origin = { origin4.x, origin4.y, origin4.z };
        // Transform the direction to the local space of the entity
        math::Vec4 direction4 = math::Inverse(transform.worldTransform) * math::Vec4(ray.direction, 0.0f);
        math::Vec3 direction = { direction4.x, direction4.y, direction4.z };

        // Store the local origin and the local direction in a localRay variable
        Ray localRay(origin, direction);

        // Now make an intersection test with the axis align bounding box
        if (RayAABoxIntersection(localRay, aabb, outRaycastHit.hitInfo))
        {
            return true;
        }

        return false;
    }

    math::Vec4 BuildPlane(const math::Vec3& p_point1, const math::Vec3& p_normal)
    {
        math::Vec4 res;
        math::Vec3 normal = math::Normalized(p_normal);
        res.w = math::Dot(normal, p_point1);
        res.x = normal.x;
        res.y = normal.y;
        res.z = normal.z;
        return res;
    }

    static float IntersectRayPlane(const math::Vec3& rOrigin, const math::Vec3& rVector, const math::Vec4& plane)
    {
        const float numer = math::Dot(math::Vec3(plane.x, plane.y, plane.z), rOrigin) - plane.w;
        const float denom = math::Dot(math::Vec3(plane.x, plane.y, plane.z), rVector);

        if (fabsf(denom) < FLT_EPSILON)  // normal is orthogonal to vector, cant intersect
        {
            return -1.0f;
        }

        return -(numer / denom);
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
        math::Vec3 worldPos = ScreenToWorldPoint(mousePos
           , cameraComp->view
           , cameraComp->proj
           , math::Vec4(0, 0, viewportRect.size.x, viewportRect.size.y));

        math::Vec3 direction = math::Normalized(worldPos - cameraComp->pos);

        Ray ray = Ray(cameraComp->pos, direction);

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& transform = transforms[i];
            auto& gizmo = gizmos[i];
            auto& mesh = meshes[i];

            AABB CalcedAABB = {};
            float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max(), minZ = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::min(), maxY = std::numeric_limits<float>::min(), maxZ = std::numeric_limits<float>::min();
            for (const auto& vert : mesh.model.GetAsset()->GetMesh().vertices)
            {
                if (vert.x < minX)
                {
                    minX = vert.x;
                }
                if (vert.y < minY)
                {
                    minY = vert.y;
                }
                if (vert.z < minZ)
                {
                    minZ = vert.z;
                }
                if (vert.x > maxX)
                {
                    maxX = vert.x;
                }
                if (vert.y > maxY)
                {
                    maxY = vert.y;
                }
                if (vert.z > maxZ)
                {
                    maxZ = vert.z;
                }
            }

            CalcedAABB = { {minX, minY, minZ}, { maxX - minX, maxY - minY, maxZ - minZ }};

            // A temporary RaycastHit variable to store the result for this entity
            RaycastHit hit;

            gizmo.mouseDown &= inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down;

            // The intersection test
            if (IntersectRayBoundingBox(ray, hit, transform, CalcedAABB))
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
                auto plane = BuildPlane(worldPos, -forward);
                const float signedLength = IntersectRayPlane(ray.origin, ray.direction, plane);
                const float len = std::abs(signedLength);
                math::Vec3 projectedPos = (ray.origin + ray.direction * len);
                switch (gizmo.axis)
                {
                    case components::GizmoAxis::Forward:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(0.f, 0.f, projectedPos.z));
                        break;
                    }
                    case components::GizmoAxis::Right:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(projectedPos.x, 0.f, 0.f));
                        break;
                    }
                    case components::GizmoAxis::Up:
                    {
                        gizmo.onMove.Broadcast(math::Vec3(0.f, projectedPos.y, 0.f));
                        break;
                    }
                }
            }
        }
    }
}
