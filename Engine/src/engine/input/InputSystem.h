#pragma once

#include "engine/ecs/System.h"
#include "InputComponent.h"

namespace se::input
{
    class InputSystem : public ecs::EngineSystem<InputComponent>
    {
        DECLARE_SPARK_SYSTEM(InputSystem)

        void OnUpdate(const std::vector<se::ecs::Id> &, InputComponent *) override;
    };
}
