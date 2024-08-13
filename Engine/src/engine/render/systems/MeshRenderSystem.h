#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/camera/ActiveCameraComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    class MeshRenderSystem : public ecs::EngineSystem<TransformComponent, const MeshComponent, camera::ActiveCameraComponent>
    {
        DECLARE_SPARK_SYSTEM(MeshRenderSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities, TransformComponent* transform, const MeshComponent* mesh, camera::ActiveCameraComponent* camera) override;
        void OnRender(const std::vector<se::ecs::Id> &entities, TransformComponent* transform, const MeshComponent* mesh, camera::ActiveCameraComponent* camera) override;
    };
}
