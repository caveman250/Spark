#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::geo::systems 
{
    class MeshAABBSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
        
        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}