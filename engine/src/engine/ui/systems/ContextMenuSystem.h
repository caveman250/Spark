#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::ui::systems 
{
    class ContextMenuSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
    };
}