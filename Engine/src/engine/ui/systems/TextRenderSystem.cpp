#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "TextRenderSystem.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/util/MeshUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(TextRenderSystem)

    void TextRenderSystem::OnRender(const std::vector<ecs::Id>& entities, const components::RectTransformComponent* transformComps, components::TextComponent* textComps, const components::WidgetComponent* widgetComps)
    {
        PROFILE_SCOPE("TextRenderSystem::OnRender")

        auto app = Application::Get();
        auto renderer = render::Renderer::Get();
        auto window = app->GetPrimaryWindow();
        auto assetManager = asset::AssetManager::Get();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& widget = widgetComps[i];
            if (!widget.renderingEnabled)
            {
                continue;
            }

            const auto& transform = transformComps[i];
            auto& text = textComps[i];
            if (!text.material)
            {
                auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
                auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/text.sass");

                text.material = render::Material::CreateMaterial({vert}, {frag});

                render::RenderState rs;
                rs.srcBlend = render::BlendMode::SrcAlpha;
                rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
                text.material->SetRenderState(rs);
            }

            if (transform.rect.topLeft != transform.lastRect.topLeft)
            {
                auto floatVec = math::Vec2(transform.rect.topLeft);
                text.material->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
            }

            if (!text.vertBuffer ||
                text.lastRect.size != transform.rect.size ||
                text.lastText != text.text ||
                text.lastFontSize != text.fontSize)
            {
                asset::StaticMesh mesh = util::CreateTextMesh(transform.rect, text.font, text.fontSize, text.text);
                text.vertBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
                text.vertBuffer->CreatePlatformResource();
                text.indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
                text.indexBuffer->CreatePlatformResource();

                if (text.lastFontSize != text.fontSize)
                {
                    auto texture = text.font->GetTextureAsset(text.fontSize);
                    text.material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &texture);
                }

                text.lastFontSize = text.fontSize;
                text.lastText = text.text;
            }

            text.lastRect = transform.rect;

            text.material->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowsSize);

            renderer->Submit<render::commands::SubmitUI>(window, text.material, text.vertBuffer, text.indexBuffer, transform.layer);
        }
    }
}
