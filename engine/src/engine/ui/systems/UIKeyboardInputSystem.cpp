#include "spark.h"

#include "UIKeyboardInputSystem.h"

#include "ResetKeyInputSystem.h"
#include "engine/input/InputUtil.h"
#include <easy/profiler.h>
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/input/InputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration UIKeyboardInputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("UIKeyboardInputSystem")
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<const RootComponent>()
                    .WithComponent<components::KeyInputComponent>()
                    .WithSingletonComponent<input::InputComponent>()
                    .WithDependency<ResetKeyInputSystem>();
    }

    void UIKeyboardInputSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("UIKeyboardInputSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
            auto* receivesInputComps = updateData.GetComponentArray<components::KeyInputComponent>();
            auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto entity = entities[i];
                auto& transform = rectTransforms[i];
                auto& inputReceiver = receivesInputComps[i];

                bool hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
                if (hovered)
                {
                    input::InputUtil::ProcessKeyEvents(entity, inputComp, [this, entity, inputComp, &inputReceiver](const input::KeyEvent& keyEvent)
                    {
                        if (TryConsumeEvent(keyEvent, inputReceiver))
                        {
                            return true;
                        }

                        bool consumed = false;
                        auto declaration = ecs::HeirachyQueryDeclaration()
                                .WithComponent<const components::RectTransformComponent>()
                                .WithComponent<components::KeyInputComponent>();
                        RunRecursiveChildQuery(entity, declaration,
                    [this, &consumed, inputComp, keyEvent](const ecs::SystemUpdateData& updateData)
                        {
                            if (consumed)
                            {
                                return true;
                            }

                            const auto& children = updateData.GetEntities();
                            auto* childTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
                            auto* childInputComps = updateData.GetComponentArray<components::KeyInputComponent>();

                            for (size_t j = 0; j < children.size(); ++j)
                            {
                                auto& childTransform = childTransforms[j];
                                auto& childInputReceiver = childInputComps[j];
                                bool childHovered = childTransform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
                                if (childHovered)
                                {
                                    if (TryConsumeEvent(keyEvent, childInputReceiver))
                                    {
                                        consumed = true;
                                        return true;
                                    }
                                }
                            }

                            return false;
                        });

                        return false;
                    });
                }
            }
        });
    }

    bool UIKeyboardInputSystem::TryConsumeEvent(const input::KeyEvent& keyEvent, components::KeyInputComponent& inputReceiver)
    {
        if (static_cast<int>(keyEvent.key) & static_cast<int>(inputReceiver.keyMask))
        {
            if (static_cast<int>(keyEvent.state) & static_cast<int>(inputReceiver.stateMask))
            {
                inputReceiver.keyEvents.push_back(keyEvent);
                return true;
            }
        }

        return false;
    }
}
