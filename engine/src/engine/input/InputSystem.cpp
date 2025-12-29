#include "InputSystem.h"

#include <easy/profiler.h>

namespace se::input
{
    ecs::SystemDeclaration InputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Input System")
            .WithSingletonComponent<InputComponent>();
    }

    void InputSystem::OnUpdate(const ecs::SystemUpdateData &updateData)
    {
        EASY_BLOCK("InputSystem::OnUpdate");

        auto* inputComp = updateData.GetSingletonComponent<InputComponent>();

        inputComp->mouseDeltaX = inputComp->mouseX - inputComp->lastMouseX;
        inputComp->mouseDeltaY = inputComp->mouseY - inputComp->lastMouseY;

        inputComp->lastMouseX = inputComp->mouseX;
        inputComp->lastMouseY = inputComp->mouseY;
    }
}
