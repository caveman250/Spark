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
                    .WithComponent<const RootComponent>()
                    .WithComponent<components::KeyInputComponent>()
                    .WithSingletonComponent<input::InputComponent>()
                    .WithDependency<ResetKeyInputSystem>();
    }

    void UIKeyboardInputSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("UIKeyboardInputSystem::OnUpdate");

        auto* world = Application::Get()->GetWorld();
        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [this, world](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* receivesInputComps = updateData.GetComponentArray<components::KeyInputComponent>();
            auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto entity = entities[i];
                auto& inputReceiver = receivesInputComps[i];

                if (world->HasComponent<ParentComponent>(entity) || inputReceiver.keyMask != input::Key::Unknown)
                {
                    input::InputUtil::ProcessKeyEvents(entity, inputComp, [this, entity, world, &inputReceiver](const input::KeyEvent& keyEvent)
                    {
                        if (TryConsumeEvent(keyEvent, inputReceiver))
                        {
                            return true;
                        }

                        bool consumed = false;
                        auto declaration = ecs::HeirachyQueryDeclaration()
                                .WithComponent<components::KeyInputComponent>();
                        RunRecursiveChildQuery(entity, declaration,
                    [this, &consumed, world, keyEvent](const ecs::SystemUpdateData& updateData)
                        {
                            if (consumed)
                            {
                                return true;
                            }

                            const auto& child = updateData.GetEntity();
                            auto* childInputComps = updateData.GetComponentArray<components::KeyInputComponent>();

                            if (world->HasComponent<ParentComponent>(child) || childInputComps->keyMask != input::Key::Unknown)
                            {
                                if (TryConsumeEvent(keyEvent, *childInputComps))
                                {
                                    consumed = true;
                                    return true;
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
