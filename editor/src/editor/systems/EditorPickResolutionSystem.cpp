#include "EditorPickResolutionSystem.h"
#include "EditorDirLightPickSystem.h"
#include "EditorMeshPickSystem.h"
#include "EditorPointLightPickSystem.h"
#include "editor/singleton_components/EditorPickComponent.h"
#include "editor/util/ViewportUtil.h"
#include "engine/camera/ActiveCameraComponent.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration EditorPickResolutionSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithSingletonComponent<singleton_components::EditorPickComponent>()
            .WithSingletonComponent<const camera::ActiveCameraComponent>()
            .WithDependency<EditorMeshPickSystem>()
            .WithDependency<EditorPointLightPickSystem>()
            .WithDependency<EditorDirLightPickSystem>();
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
