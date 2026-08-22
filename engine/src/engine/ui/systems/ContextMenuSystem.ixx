module;

#include "spark.h"

export module ContextMenuSystem;

import System;

namespace se::ui::systems 
{
    export class ContextMenuSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}