#include "spark.h"

#include "ResetKeyInputSystem.h"

#include "engine/profiling/Profiler.h"
#include "engine/ui/components/KeyInputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ResetKeyInputSystem)

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
