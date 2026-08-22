module;

#include "spark.h"

export module EditorPointLightPickSystem;

import System;

namespace se::editor::systems 
{
    export class EditorPointLightPickSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}