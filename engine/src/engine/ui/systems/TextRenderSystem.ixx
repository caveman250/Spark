export module TextRenderSystem;

import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class TextRenderSystem
            : public ecs::EngineSystem
    {
    SPARK_SYSTEM()

    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
        void OnRender(const ecs::QueryResults&) override;
    };
}
