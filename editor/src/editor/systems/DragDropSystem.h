#pragma once 

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::editor::systems
{
    class DragDropSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM(EditorOnly)

        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults& updateData) override;
    };
}