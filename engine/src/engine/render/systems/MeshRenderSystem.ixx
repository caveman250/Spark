module;



export module MeshRenderSystem;

import System;

using namespace se;
using namespace se::ecs::components;

namespace se::render::systems
{
    class MeshRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
        void OnRender(const ecs::QueryResults&) override;
    };
}
