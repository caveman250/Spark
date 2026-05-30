#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::editor::systems 
{
    class EditorMeshPickSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}