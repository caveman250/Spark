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

    ecs::UpdateMode TextRenderSystem::GetUpdateMode() const
    {
        auto renderer = render::Renderer::Get<render::Renderer>();
        return renderer->SupportsMultiThreadedRendering() ? ecs::UpdateMode::MultiThreaded : ecs::UpdateMode::SingleThreaded;
    }

    void TextRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("TextRenderSystem::OnRender");

        auto app = Application::Get();
        auto window = app->GetWindow();
        auto renderer = render::Renderer::Get<render::Renderer>();

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
    }
}
