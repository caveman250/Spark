#pragma once

#include "engine/ecs/System.h"
#include "InputComponent.h"

namespace se::input
{
    class InputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnUpdate(const ecs::SystemUpdateData &updateData) override;
    };
}
