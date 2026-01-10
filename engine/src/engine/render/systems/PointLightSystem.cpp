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

    void PointLightSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
        const auto* pointLights = updateData.GetComponentArray<const components::PointLightComponent>();
        auto* renderer = Renderer::Get<Renderer>();

        ecs::util::ForEachEntity(this, updateData,
        [transforms, pointLights, renderer](size_t i)
        {
            renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
        });
    }
}
