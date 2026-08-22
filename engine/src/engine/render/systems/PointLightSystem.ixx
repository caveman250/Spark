module;

#include "spark.h"


export module PointLightSystem;
import System;

namespace se::render::systems
{
    class PointLightSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
#if SPARK_EDITOR
        void OnRender(const ecs::QueryResults&) override;
#endif
    };
}
