#include "PointLightSystem.h"

#include "engine/render/Renderer.h"
#include "engine/render/components/PointLightComponent.h"

namespace se::render::systems
{
    void PointLightSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        const auto* transforms = updateData.GetComponentArray<const TransformComponent>();
        const auto* pointLights = updateData.GetComponentArray<const components::PointLightComponent>();
        auto* renderer = Renderer::Get<Renderer>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
        }
    }
}
