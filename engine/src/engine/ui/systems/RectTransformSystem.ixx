module;

#include "spark.h"

export module RectTransformSystem;
import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class RectTransformSystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
