#include "DragDropSystem.h"

#include "engine/input/InputComponent.h"
#include "editor/singleton_components/DragDropStateComponent.h"
#include "editor/components/DragDropComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "platform/IWindow.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration DragDropSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<components::DragDropComponent>()
            .WithComponent<se::ui::components::RectTransformComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithSingletonComponent<singleton_components::DragDropStateComponent>();
    }

    void DragDropSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto app = Application::Get();
        auto window = app->GetWindow();
        auto world = app->GetWorld();
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [window, world](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();
            auto* dragDropState = updateData.GetSingletonComponent<singleton_components::DragDropStateComponent>();
            auto* dragDropComps = updateData.GetComponentArray<components::DragDropComponent>();
            auto* rectTransforms = updateData.GetComponentArray<se::ui::components::RectTransformComponent>();

            float reciprocal = 1.f / window->GetContentScale();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& rectTransform = rectTransforms[i];
                auto& dragDrop = dragDropComps[i];

                if (!dragDrop.pendingDestroy && inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] != input::KeyState::Down)
                {
                    dragDrop.pendingDestroy = true;
                    continue;
                }

                if (dragDrop.pendingDestroy)
                {
                    world->DestroyEntity(entity);
                    dragDropState->dragDropAsset = nullptr;
                }

                rectTransform.minX = (inputComp->mouseX) * reciprocal - 40;
                rectTransform.maxX = (inputComp->mouseX) * reciprocal + 40;
                rectTransform.minY = (inputComp->mouseY) * reciprocal - 40;
                rectTransform.maxY = (inputComp->mouseY) * reciprocal + 40;
            }
        });
    }
}
