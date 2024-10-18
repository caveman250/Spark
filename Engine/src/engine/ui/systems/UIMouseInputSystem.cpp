#include "spark.h"

#include "UIMouseInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIMouseInputSystem)

    void UIMouseInputSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        const RootComponent*,
        components::ReceivesMouseEventsComponent* receivesInputComps,
        input::InputComponent* inputComp)
    {
        PROFILE_SCOPE("UIMouseInputSystem::OnUpdate")

        if (inputComp->mouseDeltaX == 0 &&
            inputComp->mouseDeltaY == 0 &&
            inputComp->mouseEvents.empty())
        {
            return;
        }

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto entity = entities[i];
            auto& inputReceiver = receivesInputComps[i];

            if (!inputReceiver.hovered)
            {
                continue;
            }

            input::InputUtil::ProcessMouseEvents(inputComp, [this, entity, inputComp, &inputReceiver](const input::MouseEvent& mouseEvent)
            {
                if (TryConsumeEvent(mouseEvent, inputReceiver))
                {
                    return true;
                }

                bool consumed = false;
                RunRecursiveChildQuery<components::ReceivesMouseEventsComponent>(entity,
                [this, &consumed, inputComp, mouseEvent](const std::vector<ecs::Id>& children,components::ReceivesMouseEventsComponent* childInputComps)
                {
                    if (consumed)
                    {
                        return true;
                    }

                    for (size_t j = 0; j < children.size(); ++j)
                    {
                        auto& childInputReceiver = childInputComps[j];

                        if (childInputReceiver.hovered)
                        {
                            if (TryConsumeEvent(mouseEvent, childInputReceiver))
                            {
                                consumed = true;
                                return true;
                            }
                        }
                    }

                    return false;
                });

                return consumed;
            });
        }
    }

    bool UIMouseInputSystem::TryConsumeEvent(const input::MouseEvent& mouseEvent, components::ReceivesMouseEventsComponent& inputReceiver)
    {
        if (mouseEvent.button & inputReceiver.buttonMask)
        {
            if (mouseEvent.state & inputReceiver.stateMask)
            {
                inputReceiver.mouseEvents.push_back(mouseEvent);
                return true;
            }
        }

        return false;
    }
}
