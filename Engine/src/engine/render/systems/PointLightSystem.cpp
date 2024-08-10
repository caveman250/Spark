#include "PointLightSystem.h"

#include "engine/render/Renderer.h"

namespace se::render::systems
{
    DEFINE_SPARK_SYSTEM(PointLightSystem)

    void PointLightSystem::OnUpdate(const std::vector<ecs::EntityId>& entities, components::PointLightComponent* pointLights, ecs::components::TransformComponent* transforms)
    {
        auto* renderer = Renderer::Get();
        for (size_t i = 0; i < entities.size(); ++i)
        {
            renderer->AddPointLight( PointLight{ transforms[i].pos, pointLights[i].color });
        }
    }
}
