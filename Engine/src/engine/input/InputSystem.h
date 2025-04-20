#pragma once

#include "engine/ecs/System.h"
#include "InputComponent.h"

namespace se::input
{
    class InputSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(InputSystem)

        void OnUpdate(const ecs::SystemUpdateData &updateData) override;
    };
}
