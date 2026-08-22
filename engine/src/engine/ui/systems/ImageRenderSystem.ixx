export module ImageRenderSystem;
import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class ImageRenderSystem
            : public ecs::EngineSystem
    {
    SPARK_SYSTEM()

    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnRender(const ecs::QueryResults&) override;
    };
}
