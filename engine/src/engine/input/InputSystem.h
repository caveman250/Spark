#pragma once

#include "engine/ecs/System.h"
#include "InputComponent.h"

namespace se::input
{
    class InputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults& results) override;
    };
}
