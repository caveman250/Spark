export module UIRenderSystem;

import System;

namespace se::ui::systems
{
    export class UIRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnRender(const ecs::QueryResults&) override;

    };
}
