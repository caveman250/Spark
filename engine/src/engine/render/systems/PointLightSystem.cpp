#include "PointLightSystem.h"

#include "engine/render/Renderer.h"
#include "engine/render/components/PointLightComponent.h"
#include "engine/ecs/components/TransformComponent.h"

namespace se::render::systems
{
    ecs::SystemDeclaration PointLightSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("PointLightSystem")
                .WithComponent<const components::PointLightComponent>()
                .WithComponent<const ecs::components::TransformComponent>();
    }

    void PointLightSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto renderer = Renderer::Get<Renderer>();

#if SPARK_EDITOR
        auto* editor = Application::Get()->GetEditor();
        if (editor->GetMode() == editor::EditorMode::Prefab)
        {
            renderer->AddPointLight( PointLight{ math::Vec3(-5, 5, 5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            renderer->AddPointLight( PointLight{ math::Vec3(5, 5, 5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            renderer->AddPointLight( PointLight{ math::Vec3(0, 5, -5), math::Vec4(1.f, 1.f, 1.f, 1.f) });
            return;
        }
#endif

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
            const auto* pointLights = updateData.GetComponentArray<const components::PointLightComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
            }
        });
    }
}
