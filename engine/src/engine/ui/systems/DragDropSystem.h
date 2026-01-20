#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::ui::systems 
{
    class DragDropSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}