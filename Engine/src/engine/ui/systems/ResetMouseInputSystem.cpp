#include "spark.h"

#include "ResetMouseInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

#if SPARK_EDITOR
#include "editor/util/ViewportUtil.h"
#endif

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ResetMouseInputSystem)

    void ResetMouseInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ResetMouseInputSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<components::MouseInputComponent>();
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

#if SPARK_EDITOR
            if (!entities[i].HasFlag(ecs::IdFlags::Editor))
            {
                auto adjustedMousePos = editor::util::ScreenSpaceToGameViewportSpace(inputComp->mouseX, inputComp->mouseY);
                inputReceiver.hovered = transform.rect.Contains(adjustedMousePos);
            }
            else
#endif
            {
                inputReceiver.hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
            }

        }
    }
}
