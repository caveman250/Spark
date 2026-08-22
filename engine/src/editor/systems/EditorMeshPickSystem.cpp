module;







#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
#include "platform/IWindow.h"
#include "engine/ui/components/RectTransformComponent.h"



module EditorMeshPickSystem;
import RotationGizmoSystem;
import TranslationGizmoSystem;
import TransformGizmoComponent;
import MeshComponent;

namespace se::editor::systems
{
    ecs::SystemDeclaration EditorMeshPickSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<ecs::components::TransformComponent>()
            .WithComponent<ecs::components::MeshComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithSingletonComponent<singleton_components::EditorPickComponent>()
            .WithDependency<TranslationGizmoSystem>()
            .WithDependency<RotationGizmoSystem>();
    }

    void EditorMeshPickSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto editor = Application::Get()->GetEditor();
        if (editor->InGameMode())
        {
            return;
        }

        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [editor](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto cameraComp = updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();
            auto inputComp = updateData.GetSingletonComponent<input::InputComponent>();
            auto pickComp = updateData.GetSingletonComponent<singleton_components::EditorPickComponent>();
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* meshes = updateData.GetComponentArray<ecs::components::MeshComponent>();

            auto ray = util::GetEditorMouseRay(inputComp, cameraComp);

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                if (editor->GetMode() == EditorMode::Prefab && *entity.scene != editor->GetPrefabEditorScene())
                {
                    continue;
                }

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

                auto hit = geo::util::RayCastAABB(ray, transform);
                if (hit.has_value())
                {
                    input::InputUtil::ProcessMouseEvents(entity, inputComp, [entity, pickComp, hit](const input::MouseEvent& mouseEvent)
                    {
                        if (mouseEvent.button == input::MouseButton::Left)
                        {
                            if (mouseEvent.state == input::KeyState::Down)
                            {
                                pickComp->results.push_back({entity, hit.value().intersectionPoint });
                                return false;
                            }
                        }

                        return false;
                    });
                }
            }
        });
    }
}
