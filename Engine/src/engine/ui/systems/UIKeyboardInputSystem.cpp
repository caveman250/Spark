#include "spark.h"

#include "UIKeyboardInputSystem.h"

#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIKeyboardInputSystem)

    void UIKeyboardInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("UIKeyboardInputSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        const auto* rectTransforms = updateData.GetComponentArray<const ui::components::RectTransformComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<ui::components::KeyInputComponent>();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();


        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto entity = entities[i];
            auto& transform = rectTransforms[i];
            auto& inputReceiver = receivesInputComps[i];

            bool hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
            if (hovered)
            {
                input::InputUtil::ProcessKeyEvents(inputComp, [this, entity, inputComp, &inputReceiver](const input::KeyEvent& keyEvent)
                {
                    if (TryConsumeEvent(keyEvent, inputReceiver))
                    {
                        return true;
                    }

                    bool consumed = false;
                    auto declaration = ecs::ChildQueryDeclaration()
                            .WithComponent<components::RectTransformComponent>()
                            .WithComponent<components::KeyInputComponent>();
                    RunRecursiveChildQuery(entity, declaration,
                [this, &consumed, inputComp, keyEvent](const ecs::SystemUpdateData& updateData)
                    {
                        if (consumed)
                        {
                            return true;
                        }

                        const auto& children = updateData.GetEntities();
                        auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
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
    }

    bool UIKeyboardInputSystem::TryConsumeEvent(const input::KeyEvent& keyEvent, components::KeyInputComponent& inputReceiver)
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
