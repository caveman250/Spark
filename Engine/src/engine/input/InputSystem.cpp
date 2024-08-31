#include "InputSystem.h"

#include "engine/profiling/Profiler.h"

namespace se::input
{
    DEFINE_SPARK_SYSTEM(InputSystem)

    void InputSystem::OnUpdate(const std::vector<se::ecs::Id> &, InputComponent* inputComp)
    {
        PROFILE_SCOPE("InputSystem::OnUpdate")

        inputComp->mouseDeltaX = inputComp->mouseX - inputComp->lastMouseX;
        inputComp->mouseDeltaY = inputComp->mouseY - inputComp->lastMouseY;

        inputComp->lastMouseX = inputComp->mouseX;
        inputComp->lastMouseY = inputComp->mouseY;
    }
}
