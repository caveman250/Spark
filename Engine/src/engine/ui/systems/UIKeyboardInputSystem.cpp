#include "spark.h"

#include "UIKeyboardInputSystem.h"

#include "engine/input/InputUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIKeyboardInputSystem)

    void UIKeyboardInputSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        const components::RectTransformComponent* rectTransforms,
        const RootComponent*,
        components::ReceivesKeyboardEventsComponent* receivesInputComps,
        input::InputComponent* inputComp)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto entity = entities[i];
            auto& transform = rectTransforms[i];
            auto& inputReceiver = receivesInputComps[i];

            inputReceiver.keyEvents.clear();
            RunRecursiveChildQuery<components::ReceivesKeyboardEventsComponent>(entity,
            [](const std::vector<ecs::Id>& children, components::ReceivesKeyboardEventsComponent* childInputComps)
            {
                for (size_t j = 0; j < children.size(); ++j)
                {
                    auto& childInputReceiver = childInputComps[j];
                    childInputReceiver.keyEvents.clear();
                }
            });

            bool hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
            if (hovered)
            {
                input::InputUtil::ProcessKeyEvents(inputComp, [this, entity, inputComp, &inputReceiver](const input::KeyEvent& keyEvent)
                {
                    if (TryConsumeEvent(entity, keyEvent, inputReceiver))
                    {
                        return true;
                    }

                    bool consumed = false;
                    RunRecursiveChildQuery<components::RectTransformComponent, components::ReceivesKeyboardEventsComponent>(entity, [this, &consumed, inputComp, keyEvent](const std::vector<ecs::Id>& children,
                        const components::RectTransformComponent* childTransforms,
                        components::ReceivesKeyboardEventsComponent* childInputComps)
                    {
                        if (consumed)
                        {
                            return;
                        }

                        for (size_t j = 0; j < children.size(); ++j)
                        {
                            auto childEntity = children[j];
                            auto& childTransform = childTransforms[j];
                            auto& childInputReceiver = childInputComps[j];
                            bool childHovered = childTransform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
                            if (childHovered)
                            {
                                if (TryConsumeEvent(childEntity, keyEvent, childInputReceiver))
                                {
                                    consumed = true;
                                    return;
                                }
                            }
                        }
                    });

                    return false;
                });
            }
        }
    }

    bool UIKeyboardInputSystem::TryConsumeEvent(ecs::Id entity, const input::KeyEvent& keyEvent, components::ReceivesKeyboardEventsComponent& inputReceiver)
    {
        if (keyEvent.key & inputReceiver.keyMask)
        {
            if (keyEvent.state & inputReceiver.stateMask)
            {
                inputReceiver.keyEvents.push_back(keyEvent);
                return true;
            }
        }

        return false;
    }
}
