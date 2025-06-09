#include "UIRenderSystem.h"
#include "engine/render/Renderer.h"

namespace se::ui::systems
{
    void RenderEntity(const ecs::Id& id, singleton_components::UIRenderComponent* renderComp, render::Renderer* renderer, IWindow* window, ecs::World* world)
    {
#if SPARK_EDITOR
        auto* editorRuntime = Application::Get()->GetEditorRuntime();
        bool isEditor = id.HasFlag(ecs::IdFlags::Editor);
        size_t group = isEditor ? renderer->GetDefaultRenderGroup() : editorRuntime->GetOffscreenRenderGroup();
#else
        size_t group = renderer->GetDefaultRenderGroup();
#endif

        if (renderComp->entityPreRenderCommands.contains(id))
        {
            for (auto *renderCommand: renderComp->entityPreRenderCommands.at(id))
            {
                renderer->Submit(group, renderCommand);
            }

            renderComp->entityPreRenderCommands.at(id).clear();
        }

        if (renderComp->entityRenderCommands.contains(id))
        {
            for (auto *renderCommand: renderComp->entityRenderCommands.at(id))
            {
                renderer->Submit(group, renderCommand);
            }

            renderComp->entityRenderCommands.at(id).clear();
        }

        for (const auto& child : world->GetChildren(id))
        {
            RenderEntity(child, renderComp, renderer, window, world);
        }

        if (renderComp->entityPostRenderCommands.contains(id))
        {
            for (auto *renderCommand: renderComp->entityPostRenderCommands.at(id))
            {
                renderer->Submit(group, renderCommand);
            }

            renderComp->entityPostRenderCommands.at(id).clear();
        }
    }

    void UIRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        auto* primaryWindow = app->GetPrimaryWindow();
        auto* renderer = render::Renderer::Get<render::Renderer>();
        const auto& entities = updateData.GetEntities();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();
        for (const auto& entity : entities)
        {
            RenderEntity(entity, renderComp, renderer, primaryWindow, world);
        }
    }
}