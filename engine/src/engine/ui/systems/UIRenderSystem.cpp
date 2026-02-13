#include "UIRenderSystem.h"

#include "CollapsingHeaderSystem.h"
#include "EditableTextSystem.h"
#include "ImageRenderSystem.h"
#include "ScrollBoxRenderSystem.h"
#include "TextRenderSystem.h"
#include "engine/render/Renderer.h"

namespace se::ui::systems
{
    size_t GetGroupForLayer(const UILayerKey& layer,
                            singleton_components::UIRenderComponent* rendComp)
    {
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto it = rendComp->LayerToRenderGroupMap.find(layer);
        if (it == rendComp->LayerToRenderGroupMap.end())
        {
            it = rendComp->LayerToRenderGroupMap.insert(std::make_pair(layer, renderer->AllocRenderGroup(layer.layer))).first;
#if SPARK_EDITOR
            if (!layer.editor)
            {
                editor::EditorRuntime* editor = Application::Get()->GetEditorRuntime();
                renderer->SetFrameBuffer(it->second, editor->GetFrameBuffer());
            }
#endif
        }

        return it->second;
    }

    void RenderEntity(const ecs::Id& id,
                      singleton_components::UIRenderComponent* renderComp,
                      render::Renderer* renderer,
                      IWindow* window,
                      ecs::World* world)
    {
        if (renderComp->entityPreRenderCommands.contains(id))
        {
            for (const auto& renderCommand: renderComp->entityPreRenderCommands.at(id))
            {
                renderer->Submit(GetGroupForLayer(renderCommand.layer, renderComp), renderCommand.renderCmd);
            }

            renderComp->entityPreRenderCommands.at(id).clear();
        }

        if (renderComp->entityRenderCommands.contains(id))
        {
            for (const auto& renderCommand: renderComp->entityRenderCommands.at(id))
            {
                renderer->Submit(GetGroupForLayer(renderCommand.layer, renderComp), renderCommand.renderCmd);
            }

            renderComp->entityRenderCommands.at(id).clear();
        }

        for (const auto& child: world->GetChildren(id))
        {
            RenderEntity(child, renderComp, renderer, window, world);
        }

        if (renderComp->entityPostRenderCommands.contains(id))
        {
            for (const auto& renderCommand: renderComp->entityPostRenderCommands.at(id))
            {
                renderer->Submit(GetGroupForLayer(renderCommand.layer, renderComp), renderCommand.renderCmd);
            }

            renderComp->entityPostRenderCommands.at(id).clear();
        }
    }

    ecs::SystemDeclaration UIRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("UI Render System")
                    .WithComponent<const RootComponent>()
                    .WithComponent<const components::WidgetComponent>()
                    .WithSingletonComponent<singleton_components::UIRenderComponent>()
                    .WithDependency<ImageRenderSystem>()
                    .WithDependency<TextRenderSystem>()
                    .WithDependency<EditableTextSystem>()
                    .WithDependency<ScrollBoxRenderSystem>()
                    .WithDependency<CollapsingHeaderSystem>();
    }

    void UIRenderSystem::OnRender(const ecs::QueryResults& results)
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();
        auto* primaryWindow = app->GetWindow();
        auto* renderer = render::Renderer::Get<render::Renderer>();

        ecs::ForEachArcheType(results, ecs::UpdateMode::SingleThreaded, false, [world, primaryWindow, renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

            renderComp->LayerToRenderGroupMap.clear();
    #if SPARK_EDITOR
            auto* editorRuntime = Application::Get()->GetEditorRuntime();
            renderComp->LayerToRenderGroupMap.insert(std::make_pair(UILayerKey(0), editorRuntime->GetOffscreenRenderGroup()));
            renderComp->LayerToRenderGroupMap.insert(std::make_pair(UILayerKey(0, true), renderer->GetDefaultRenderGroup()));
    #else
            renderComp->LayerToRenderGroupMap.insert(std::make_pair(UILayerKey(0), renderer->GetDefaultRenderGroup()));
    #endif

            for (const auto& entity: entities)
            {
                RenderEntity(entity, renderComp, renderer, primaryWindow, world);
            }
        });
    }
}
