#include "spark.h"

#include "UIMouseInputSystem.h"
#include <easy/profiler.h>
#include "ResetMouseInputSystem.h"
#include "RootRectTransformSystem.h"
#include "editor/util/ViewportUtil.h"
#include "engine/ecs/util/SystemUtil.h"
#include "engine/input/InputUtil.h"
#include "engine/ui/components/RectTransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration UIMouseInputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("UIMouseInputSystem")
                    .WithComponent<const RootComponent>()
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<components::MouseInputComponent>()
                    .WithSingletonComponent<input::InputComponent>()
                    .WithDependency<ResetMouseInputSystem>()
                    .WithDependency<RootRectTransformSystem>();
    }



    void UIMouseInputSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("UIMouseInputSystem::OnUpdate");

        input::InputComponent* inputComp = nullptr;
        struct InputRootEntity
        {
            ecs::Id id = {};
            components::MouseInputComponent* mouseInput = nullptr;
            const components::RectTransformComponent* rectTransform = nullptr;
        };
        std::vector<InputRootEntity> rootEntities = {};

        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [&rootEntities, &inputComp](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            if (!inputComp)
            {
                inputComp = updateData.GetSingletonComponent<input::InputComponent>();
            }
            auto* receivesInputComps = updateData.GetComponentArray<components::MouseInputComponent>();
            const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();

            if (inputComp->mouseDeltaX == 0 &&
                inputComp->mouseDeltaY == 0 &&
                inputComp->mouseEvents.empty())
            {
                return;
            }

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto entity = entities[i];
                auto& inputReceiver = receivesInputComps[i];
                const auto& rectTransform = rectTransforms[i];

                if (!inputReceiver.hovered)
                {
                    continue;
                }

                if (!inputReceiver.enabled)
                {
                    continue;
                }

#if SPARK_EDITOR
                const auto editor = Application::Get()->GetEditorRuntime();
                if (*entity.scene == editor->GetEditorScene() && editor::util::PosWithinViewport(inputComp->mouseX, inputComp->mouseY))
                {
                    continue;
                }
#endif

                rootEntities.push_back({ entity, &inputReceiver, &rectTransform });
            }
        });

        std::ranges::sort(rootEntities, [](const auto& a, const auto& b)
        {
            return a.rectTransform->layer < b.rectTransform->layer;
        });

        for (const auto& entity : rootEntities)
        {
            input::InputUtil::ProcessMouseEvents(entity.id, inputComp, [this, &entity](const input::MouseEvent& mouseEvent)
            {
                bool consumed = false;
                const auto declaration = ecs::HeirachyQueryDeclaration()
                        .WithComponent<components::MouseInputComponent>();
                RunRecursiveChildQuery(entity.id, declaration,
                [&consumed, mouseEvent](const ecs::SystemUpdateData& updateData)
                {
                    if (consumed)
                    {
                        return true;
                    }

                    [[maybe_unused]] const auto& child = updateData.GetEntity();
                    auto* childInputComp = updateData.GetComponentArray<components::MouseInputComponent>();

                    if (childInputComp->hovered && childInputComp->enabled)
                    {
                        if (TryConsumeEvent(mouseEvent, *childInputComp))
                        {
                            consumed = true;
                            return true;
                        }
                    }

                    return false;
                });

                if (!consumed)
                {
                    if (TryConsumeEvent(mouseEvent, *entity.mouseInput))
                    {
                        return true;
                    }
                }

                return consumed;
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
