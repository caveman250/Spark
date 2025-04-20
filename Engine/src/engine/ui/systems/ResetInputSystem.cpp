#include "spark.h"

#include "ResetInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ResetInputSystem)

    void ResetInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ResetInputSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<components::ReceivesMouseEventsComponent>();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& transform = rectTransforms[i];
            auto& inputReceiver = receivesInputComps[i];
            inputReceiver.mouseEvents.clear();

            if (inputComp->mouseDeltaX == 0 &&
                inputComp->mouseDeltaY == 0)
            {
                continue;
            }

            inputReceiver.lastHovered = inputReceiver.hovered;
            inputReceiver.hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
        }
    }
}
