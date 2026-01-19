#include "spark.h"

#include "ResetKeyInputSystem.h"

#include "engine/input/InputSystem.h"
#include <easy/profiler.h>
#include "engine/ui/components/KeyInputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ResetKeyInputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Reset Key Input System")
                    .WithComponent<components::KeyInputComponent>()
                    .WithDependency<input::InputSystem>();
    }

    void ResetKeyInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("ResetKeyInputSystem::OnUpdate");

        auto* receivesInputComps = updateData.GetComponentArray<components::KeyInputComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            auto& inputReceiver = receivesInputComps[i];
            inputReceiver.keyEvents.clear();
        }
    }
}
