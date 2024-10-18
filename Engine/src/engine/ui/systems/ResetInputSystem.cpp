#include "spark.h"

#include "ResetInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ResetInputSystem)

    void ResetInputSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                    const components::RectTransformComponent* rectTransforms,
                                      components::ReceivesMouseEventsComponent* receivesInputComps,
                                      input::InputComponent* inputComp)
    {
        PROFILE_SCOPE("ResetInputSystem::OnUpdate")

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
