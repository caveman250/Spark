module;

#include "spark.h"


export module DirLightSystem;

import System;

namespace se::render::systems
{
    export class DirLightSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
#if SPARK_EDITOR
        void OnRender(const ecs::QueryResults&) override;
#endif
    };
}
