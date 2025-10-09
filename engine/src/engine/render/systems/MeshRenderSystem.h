#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    class MeshRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
