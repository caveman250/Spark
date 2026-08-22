export module ResetKeyInputSystem;

import System;

namespace se::ui::systems
{
    export class ResetKeyInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
