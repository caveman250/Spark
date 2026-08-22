export module ComboBoxSystem;

import System;

namespace se::ui::systems
{
    export class ComboBoxSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
