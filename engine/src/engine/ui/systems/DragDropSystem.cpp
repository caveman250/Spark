#include "DragDropSystem.h"

#include "engine/input/InputComponent.h"
#include "engine/ui/components/DragDropComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/singleton_components/DragDropStateComponent.h"
#include "platform/IWindow.h"

namespace se::ui::systems
{
    ecs::SystemDeclaration DragDropSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<components::DragDropComponent>()
            .WithComponent<components::RectTransformComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithSingletonComponent<singleton_components::DragDropStateComponent>();
    }

    void DragDropSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        auto app = Application::Get();
        auto window = app->GetWindow();
        auto world = app->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();
        auto* dragDropState = updateData.GetSingletonComponent<singleton_components::DragDropStateComponent>();
        auto* dragDropComps = updateData.GetComponentArray<components::DragDropComponent>();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

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
    }
}
