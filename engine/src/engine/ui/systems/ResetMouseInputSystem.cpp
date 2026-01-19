#include "spark.h"

#include "ResetMouseInputSystem.h"

#include "engine/input/InputSystem.h"
#include "engine/input/InputUtil.h"
#include <easy/profiler.h>
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

#if SPARK_EDITOR
#include "editor/util/ViewportUtil.h"
#endif

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ResetMouseInputSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Reset Mouse Input System")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<ui::components::MouseInputComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithSingletonComponent<input::InputComponent>()
                .WithDependency<input::InputSystem>();
    }

    void ResetMouseInputSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("ResetMouseInputSystem::OnUpdate");

        const auto& entities = updateData.GetEntities();
        const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
        const auto* widgets = updateData.GetComponentArray<const components::WidgetComponent>();
        auto* receivesInputComps = updateData.GetComponentArray<components::MouseInputComponent>();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& transform = rectTransforms[i];
            const auto& widget = widgets[i];
            auto& inputReceiver = receivesInputComps[i];
            inputReceiver.mouseEvents.clear();

            if (inputComp->mouseDeltaX == 0 &&
                inputComp->mouseDeltaY == 0)
            {
                continue;
            }

            inputReceiver.lastHovered = inputReceiver.hovered;

            if (!widget.updateEnabled || !widget.parentUpdateEnabled)
            {
                inputReceiver.hovered = false;
                continue;
            }

#if SPARK_EDITOR
            if (!ecs::IsEditorEntity(entities[i]))
            {
                auto adjustedMousePos = editor::util::ScreenSpaceToGameViewportSpace(inputComp->mouseX, inputComp->mouseY);
                inputReceiver.hovered = transform.rect.Contains(adjustedMousePos);
            }
            else
#endif
            {
                inputReceiver.hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));
            }
        }
    }
}
