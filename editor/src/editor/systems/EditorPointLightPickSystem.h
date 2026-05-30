#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::editor::systems 
{
    class EditorPointLightPickSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}