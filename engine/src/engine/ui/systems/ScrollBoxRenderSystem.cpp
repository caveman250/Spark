#include "ScrollBoxRenderSystem.h"

#include <easy/profiler.h>
#include "engine/render/Renderer.h"
#include "engine/render/RenderCommand.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ScrollBoxRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("ScrollBox Render System")
                    .WithComponent<components::ScrollBoxComponent>()
                    .WithComponent<const components::RectTransformComponent>()
                    .WithSingletonComponent<singleton_components::UIRenderComponent>();
    }

    void ScrollBoxRenderSystem::OnRender(const ecs::QueryResults& results)
    {
        EASY_BLOCK("ScrollBoxRenderSystem::OnRender");

        auto renderer = render::Renderer::Get<render::Renderer>();

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            const auto* rectTransforms = updateData.GetComponentArray<const components::RectTransformComponent>();
            auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                const auto& rect = rectTransforms[i];
                renderComp->mutex.lock();
                renderComp->entityPreRenderCommands[entity].push_back(UIRenderCommand(renderer->AllocRenderCommand<render::commands::PushScissor>(rect.rect), UILayerKey(rect.layer, ecs::IsEditorEntity(entity))));
                renderComp->entityPostRenderCommands[entity].push_back(UIRenderCommand(renderer->AllocRenderCommand<render::commands::PopScissor>(), UILayerKey(rect.layer, ecs::IsEditorEntity(entity))));
                renderComp->mutex.unlock();
            }
        });
    }
}
