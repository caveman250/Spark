export module InputSystem;

import System;

namespace se::input
{
    export class InputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults& results) override;
    };
}
