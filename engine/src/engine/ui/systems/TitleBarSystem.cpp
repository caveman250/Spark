#include "spark.h"

#include "TitleBarSystem.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void TitleBarSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("TitleBarSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* titleBars = updateData.GetComponentArray<components::TitleBarComponent>();
        const auto* mouseEventComps = updateData.GetComponentArray<const components::MouseInputComponent>();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& titleBar = titleBars[i];
            auto mouseEventComp = mouseEventComps[i];

            for (const auto& event : mouseEventComp.mouseEvents)
            {
                if (event.button == input::MouseButton::Left)
                {
                    if (event.state == input::KeyState::Down)
                    {
                        titleBar.pressed = true;
                    }
                    else if (event.state == input::KeyState::Up)
                    {
                        titleBar.pressed = false;
                    }
                }
            }

            if (titleBar.pressed && (inputComp->mouseDeltaX || inputComp->mouseDeltaY))
            {
                titleBar.onMove.Broadcast(static_cast<float>(inputComp->mouseDeltaX), static_cast<float>(inputComp->mouseDeltaY));
            }

            titleBar.lastPressed = titleBar.pressed;
        }
    }
}
