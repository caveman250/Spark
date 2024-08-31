#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "TitleBarSystem.h"

#include "engine/Application.h"
#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(TitleBarSystem)

    void TitleBarSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        components::TitleBarComponent* titleBars,
        input::InputComponent* inputComp,
        const components::ReceivesMouseEventsComponent* mouseEventComps)
    {
        PROFILE_SCOPE("TitleBarSystem::OnUpdate")
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
