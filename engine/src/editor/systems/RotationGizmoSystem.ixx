module;

#include "spark.h"

export module RotationGizmoSystem;

import System;

namespace se::editor::systems 
{
    class RotationGizmoSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}