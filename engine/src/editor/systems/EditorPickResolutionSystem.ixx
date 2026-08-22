module;

#include "spark.h"

export module EditorPickResolutionSystem;

import System;

namespace se::editor::systems 
{
    export class EditorPickResolutionSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}