#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/camera/ActiveCameraComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    class MeshRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM(MeshRenderSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
