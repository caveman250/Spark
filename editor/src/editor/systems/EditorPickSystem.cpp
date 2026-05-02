#include "EditorPickSystem.h"

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
    ecs::SystemDeclaration EditorPickSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<ecs::components::TransformComponent>()
            .WithComponent<ecs::components::MeshComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithDependency<GizmoSystem>();
    }

    void EditorPickSystem::OnUpdate(const ecs::QueryResults& results)
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
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* meshes = updateData.GetComponentArray<ecs::components::MeshComponent>();

            auto mousePos = util::ScreenSpaceToGameViewportSpace(inputComp->mouseX, inputComp->mouseY);
            auto viewportRect = editor->GetViewportRect();
            math::Vec3 mouseWorldPos = util::ScreenToWorldPoint(mousePos
                                                                , cameraComp->view
                                                                , cameraComp->proj
                                                                , math::Vec4(0.f, 0.f, static_cast<float>(viewportRect.size.x), static_cast<float>(viewportRect.size.y)));
            math::Vec3 direction = math::Normalized(mouseWorldPos - cameraComp->pos);
            geo::Ray ray = geo::Ray(cameraComp->pos, direction);

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& transform = transforms[i];
                auto& mesh = meshes[i];

                // uninitialized
                if (!mesh.materialInstance)
                {
                    continue;
                }

                if (IsEditorEntity(entity))
                {
                    continue;
                }

                if (geo::util::RayCastAABB(ray, transform))
                {
                    input::InputUtil::ProcessMouseEvents(entity, inputComp, [entity](const input::MouseEvent& mouseEvent)
                    {
                        if (mouseEvent.button == input::MouseButton::Left)
                        {
                            if (mouseEvent.state == input::KeyState::Down)
                            {
                                Application::Get()->GetEditorRuntime()->SelectEntity(entity);
                                return true;
                            }
                        }

                        return false;
                    });
                }
            }
        });
    }
}
