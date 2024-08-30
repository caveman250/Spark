#include "spark.h"

#include "UIMouseInputSystem.h"

#include "engine/input/InputUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIMouseInputSystem)

    void UIMouseInputSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        const components::RectTransformComponent* rectTransforms,
        const RootComponent*,
        components::ReceivesMouseEventsComponent* receivesInputComps,
        input::InputComponent* inputComp)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto entity = entities[i];
            auto& transform = rectTransforms[i];
            auto& inputReceiver = receivesInputComps[i];

            inputReceiver.mouseEvents.clear();
            inputReceiver.lastHovered = inputReceiver.hovered;
            inputReceiver.hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
            RunRecursiveChildQuery<components::ReceivesMouseEventsComponent, const components::RectTransformComponent>(entity,
            [inputComp](const std::vector<ecs::Id>& children,
                components::ReceivesMouseEventsComponent* childInputComps,
                const components::RectTransformComponent* childRectTransforms)
            {
                for (size_t j = 0; j < children.size(); ++j)
                {
                    const auto& childTransform = childRectTransforms[j];
                    auto& childInputReceiver = childInputComps[j];
                    childInputReceiver.mouseEvents.clear();
                    childInputReceiver.lastHovered = childInputReceiver.hovered;
                    childInputReceiver.hovered = childTransform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
                }
            });

            if (inputReceiver.hovered)
            {
                input::InputUtil::ProcessMouseEvents(inputComp, [this, entity, inputComp, &inputReceiver](const input::MouseEvent& mouseEvent)
                {
                    if (TryConsumeEvent(entity, mouseEvent, inputReceiver))
                    {
                        return true;
                    }

                    bool consumed = false;
                    RunRecursiveChildQuery<components::ReceivesMouseEventsComponent>(entity,
                    [this, &consumed, inputComp, mouseEvent](const std::vector<ecs::Id>& children,components::ReceivesMouseEventsComponent* childInputComps)
                    {
                        if (consumed)
                        {
                            return;
                        }

                        for (size_t j = 0; j < children.size(); ++j)
                        {
                            auto childEntity = children[j];
                            auto& childInputReceiver = childInputComps[j];

                            if (childInputReceiver.hovered)
                            {
                                if (TryConsumeEvent(childEntity, mouseEvent, childInputReceiver))
                                {
                                    consumed = true;
                                    return;
                                }
                            }
                        }
                    });

                    return consumed;
                });
            }
        }
    }

    bool UIMouseInputSystem::TryConsumeEvent(ecs::Id entity, const input::MouseEvent& mouseEvent, components::ReceivesMouseEventsComponent& inputReceiver)
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
