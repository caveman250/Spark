#include "spark.h"

#include "ResetKeyInputSystem.h"

#include "engine/input/InputSystem.h"
#include "engine/profiling/Profiler.h"
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
        PROFILE_SCOPE("ResetKeyInputSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* receivesInputComps = updateData.GetComponentArray<components::KeyInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& inputReceiver = receivesInputComps[i];
            inputReceiver.keyEvents.clear();
        }
    }
}
