#pragma once

#include "engine/ecs/System.h"

using namespace se;

namespace se::editor::systems
{
    class EditorCameraSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };

}
