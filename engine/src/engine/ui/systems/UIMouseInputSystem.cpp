#include "spark.h"

#include "UIMouseInputSystem.h"

#include "ResetMouseInputSystem.h"
#include "engine/input/InputUtil.h"
#include <easy/profiler.h>

#include "RootRectTransformSystem.h"
#include "engine/ecs/util/SystemUtil.h"

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
                    .WithDependency<ResetMouseInputSystem>()
                    .WithDependency<RootRectTransformSystem>();
    }

    void UIMouseInputSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("UIMouseInputSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
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
                    return;
                }

                if (!inputReceiver.enabled)
                {
                    return;
                }

                input::InputUtil::ProcessMouseEvents(entity, inputComp, [this, entity, &inputReceiver](const input::MouseEvent& mouseEvent)
                {
                    bool consumed = false;
                    auto declaration = ecs::HeirachyQueryDeclaration()
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

                            if (childInputReceiver.hovered && childInputReceiver.enabled)
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

                    if (!consumed)
                    {
                        if (TryConsumeEvent(mouseEvent, inputReceiver))
                        {
                            return true;
                        }
                    }

                    return consumed;
                });
            }
        });
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
