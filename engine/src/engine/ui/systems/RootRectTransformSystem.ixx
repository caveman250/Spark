export module RootRectTransformSystem;

import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class RootRectTransformSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults& results) override;
    };
}
