module;

#include "spark.h"

export module ScrollBoxRenderSystem;
import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
class ScrollBoxRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnRender(const ecs::QueryResults&) override;
    };
}
