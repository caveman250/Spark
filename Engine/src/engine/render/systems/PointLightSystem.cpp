#include "PointLightSystem.h"

#include "engine/render/Renderer.h"

namespace se::render::systems
{
    DEFINE_SPARK_SYSTEM(PointLightSystem)

    void PointLightSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
        const auto* pointLights = updateData.GetComponentArray<const render::components::PointLightComponent>();
        auto* renderer = render::Renderer::Get<render::Renderer>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
        }
    }
}
