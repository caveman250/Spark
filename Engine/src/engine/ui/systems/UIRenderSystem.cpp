#include "UIRenderSystem.h"
#include "engine/render/Renderer.h"

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(UIRenderSystem)

    void RenderEntity(const ecs::Id& id, singleton_components::UIRenderComponent* renderComp, render::Renderer* renderer, IWindow* window, ecs::World* world)
    {
        if (renderComp->entityPreRenderCommands.contains(id))
        {
            for (auto *renderCommand: renderComp->entityPreRenderCommands.at(id))
            {
                renderer->Submit(Application::Get()->GetPrimaryWindow(), renderCommand);
            }

            renderComp->entityPreRenderCommands.at(id).clear();
        }

        if (renderComp->entityRenderCommands.contains(id))
        {
            for (auto *renderCommand: renderComp->entityRenderCommands.at(id))
            {
                renderer->Submit(Application::Get()->GetPrimaryWindow(), renderCommand);
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
                renderer->Submit(Application::Get()->GetPrimaryWindow(), renderCommand);
            }

            renderComp->entityPostRenderCommands.at(id).clear();
        }
    }

    void UIRenderSystem::OnRender(const std::vector<ecs::Id> & entities,
                                  const RootComponent *,
                                  const components::WidgetComponent *,
                                  singleton_components::UIRenderComponent* renderComp)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        auto* primaryWindow = app->GetPrimaryWindow();
        auto* renderer = render::Renderer::Get();
        for (const auto& entity : entities)
        {
            RenderEntity(entity, renderComp, renderer, primaryWindow, world);
        }
    }
}