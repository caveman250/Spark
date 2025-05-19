#include "spark.h"

#include "TextRenderSystem.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/util/MeshUtil.h"
#include "engine/render/Material.h"
#include "engine/ui/util/TextUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void TextRenderSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("TextRenderSystem::OnRender")

        auto app = Application::Get();
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto window = app->GetPrimaryWindow();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        const auto& entities = updateData.GetEntities();
        const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
        const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* textComps = updateData.GetComponentArray<components::TextComponent>();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            const auto& widget = widgetComps[i];
            const auto& transform = transformComps[i];
            auto& text = textComps[i];

            util::RenderText(entity, widget, transform, text, windowsSize, renderer, renderComp, text.text);
        }
    }
}
