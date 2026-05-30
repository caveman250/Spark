#include "EditorPickResolutionSystem.h"

#include "EditorMeshPickSystem.h"
#include "EditorPointLightPickSystem.h"

#include "RotationGizmoSystem.h"
#include "TranslationGizmoSystem.h"
#include "editor/components/TransformGizmoComponent.h"
#include "editor/singleton_components/EditorPickComponent.h"
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
    ecs::SystemDeclaration EditorPickResolutionSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithSingletonComponent<singleton_components::EditorPickComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithDependency<EditorMeshPickSystem>()
            .WithDependency<EditorPointLightPickSystem>();
    }

    void EditorPickResolutionSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto editor = Application::Get()->GetEditor();
        if (editor->InGameMode())
        {
            return;
        }

        auto* pickComp = results.updateData.GetSingletonComponent<singleton_components::EditorPickComponent>();
        if (pickComp->results.size() > 0)
        {
            auto* cameraComp = results.updateData.GetSingletonComponent<const camera::ActiveCameraComponent>();

            std::ranges::sort(pickComp->results, [cameraComp](const singleton_components::EditorPickResult& a, const singleton_components::EditorPickResult& b)
            {
                return math::MagnitudeSquared(cameraComp->pos - a.pos) < math::MagnitudeSquared(cameraComp->pos - b.pos);
            });

            Application::Get()->GetEditor()->SelectEntity(pickComp->results[0].entity);
            pickComp->results.clear();
        }
    }
}
