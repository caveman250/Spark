module;

#include "spark.h"

export module TranslationGizmoSystem;

import System;

namespace se::editor::systems 
{
    class TranslationGizmoSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}