module;

#include "spark.h"

export module EditorMeshPickSystem;

import System;

namespace se::editor::systems 
{
    export class EditorMeshPickSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}