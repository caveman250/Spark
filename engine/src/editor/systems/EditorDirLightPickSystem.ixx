module;

#include "spark.h"

export module EditorDirLightPickSystem;
import System;

namespace se::editor::systems 
{
    class EditorDirLightPickSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}