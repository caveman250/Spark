#include "spark.h"

#include "UIMouseInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIMouseInputSystem)

    void UIMouseInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("UIMouseInputSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<ui::components::MouseInputComponent>();

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

            input::InputUtil::ProcessMouseEvents(entity, inputComp, [this, entity, &inputReceiver](const input::MouseEvent& mouseEvent)
            {
                if (TryConsumeEvent(mouseEvent, inputReceiver))
                {
                    return true;
                }

                bool consumed = false;
                auto declaration = ecs::ChildQueryDeclaration()
                        .WithComponent<components::MouseInputComponent>();
                RunRecursiveChildQuery(entity, declaration,
                [this, &consumed, mouseEvent](const ecs::SystemUpdateData& updateData)
                {
                    if (consumed)
                    {
                        return true;
                    }

                    const auto& children = updateData.GetEntities();
                    auto* childInputComps = updateData.GetComponentArray<components::MouseInputComponent>();

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

    bool UIMouseInputSystem::TryConsumeEvent(const input::MouseEvent& mouseEvent, components::MouseInputComponent& inputReceiver)
    {
        if (mouseEvent.button == input::MouseButton::None || mouseEvent.button & inputReceiver.buttonMask)
        {
            if (mouseEvent.state & inputReceiver.stateMask)
            {
                if (mouseEvent.scrollDelta == 0 || inputReceiver.receivesScrollEvents)
                {
                    inputReceiver.mouseEvents.push_back(mouseEvent);
                    return true;
                }
            }
        }

        return false;
    }
}
