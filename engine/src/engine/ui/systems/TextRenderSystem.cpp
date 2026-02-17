#include "spark.h"

#include "TextRenderSystem.h"

#include "engine/Application.h"
#include <easy/profiler.h>

#include "engine/ecs/util/SystemUtil.h"
#include "engine/render/Renderer.h"
#include "engine/ui/util/TextUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration TextRenderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Text Render System")
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<components::TextComponent>()
                    .WithComponent<const components::WidgetComponent>()
                    .WithSingletonComponent<singleton_components::UIRenderComponent>();
    }

    void TextRenderSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto app = Application::Get();
        auto window = app->GetWindow();
        auto renderer = render::Renderer::Get<render::Renderer>();

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [window, renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
            const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
            auto* textComps = updateData.GetComponentArray<components::TextComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                [[maybe_unused]] const auto& entity = entities[i];
                const auto& widget = widgetComps[i];
                if (!widget.updateEnabled  || !widget.parentUpdateEnabled)
                {
                    continue;
                }

                const auto& transform = transformComps[i];
                auto& text = textComps[i];

                util::UpdateText(text, transform, text.text);
            }
        });
    }

    void TextRenderSystem::OnRender(const ecs::QueryResults& results)
    {
        EASY_BLOCK("TextRenderSystem::OnRender");

        auto app = Application::Get();
        auto window = app->GetWindow();
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto updateMode = renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;

        ecs::ForEachArcheType(results, updateMode, false, [window, renderer](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
            const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
            auto* textComps = updateData.GetComponentArray<components::TextComponent>();
            auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                const auto& widget = widgetComps[i];
                if (widget.visibility != Visibility::Visible || widget.parentVisibility != Visibility::Visible)
                {
                    continue;
                }

                const auto& transform = transformComps[i];
                auto& text = textComps[i];

                auto windowSize = ecs::IsEditorEntity(entity) ?
                        math::IntVec2(window->GetWidth(), window->GetHeight()) :
                        Application::Get()->GetGameViewportSize();

                util::RenderText(entity, widget, transform, text, windowSize, renderer, renderComp, text.text);
            }
        });
    }
}
