export module TreeNodeSystem;

import System;

namespace se::ui::systems
{
class TreeNodeSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults& results) override;
    };
}
