export module RootTransformSystem;

import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    export class RootTransformSystem : public EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const QueryResults&) override;
    };
}
