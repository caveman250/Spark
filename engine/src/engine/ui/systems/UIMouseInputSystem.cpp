#include "spark.h"

#include "UIMouseInputSystem.h"

#include "ResetMouseInputSystem.h"
#include "engine/input/InputUtil.h"
#include <easy/profiler.h>

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration UIMouseInputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("UIMouseInputSystem")
                    .WithComponent<const RootComponent>()
                    .WithComponent<components::MouseInputComponent>()
                    .WithSingletonComponent<input::InputComponent>()
                    .WithDependency<ResetMouseInputSystem>();
    }

    void UIMouseInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("UIMouseInputSystem::OnUpdate");

        const auto& entities = updateData.GetEntities();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<components::MouseInputComponent>();

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

            if (!inputReceiver.enabled)
            {
                continue;
            }

            enum class ConsumedState
            {
                Consumed,
                ConsumedByEditor,
                NotConsumed
            };

            input::InputUtil::ProcessMouseEvents(entity, inputComp, [this, entity, &inputReceiver](const input::MouseEvent& mouseEvent)
            {
                ConsumedState consumedState = ConsumedState::NotConsumed;
                auto declaration = ecs::HeirachyQueryDeclaration()
                        .WithComponent<components::MouseInputComponent>();
                RunRecursiveChildQuery(entity, declaration,
                [this, &consumedState, mouseEvent](const ecs::SystemUpdateData& updateData)
                {
                    if (consumedState != ConsumedState::NotConsumed)
                    {
                        return consumedState == ConsumedState::Consumed;
                    }

                    const auto& children = updateData.GetEntities();
                    auto* childInputComps = updateData.GetComponentArray<components::MouseInputComponent>();

                    for (size_t j = 0; j < children.size(); ++j)
                    {
                        const auto& child = children[j];
                        auto& childInputReceiver = childInputComps[j];

                        if (childInputReceiver.hovered && childInputReceiver.enabled)
                        {
                            if (TryConsumeEvent(mouseEvent, childInputReceiver))
                            {
                                bool isEditorWidget = ecs::IsEditorEntity(child);
                                consumedState = isEditorWidget ? ConsumedState::ConsumedByEditor : ConsumedState::Consumed;
                                return !isEditorWidget;
                            }
                        }
                    }

                    return false;
                });

                if (consumedState == ConsumedState::NotConsumed)
                {
                    if (TryConsumeEvent(mouseEvent, inputReceiver))
                    {
                        return !ecs::IsEditorEntity(entity);
                    }
                }

                return consumedState == ConsumedState::Consumed;
            });
        }
    }

    bool UIMouseInputSystem::TryConsumeEvent(const input::MouseEvent& mouseEvent, components::MouseInputComponent& inputReceiver)
    {
        if (mouseEvent.button == input::MouseButton::None || static_cast<int>(mouseEvent.button) & inputReceiver.buttonMask)
        {
            if (static_cast<int>(mouseEvent.state) & inputReceiver.stateMask)
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
