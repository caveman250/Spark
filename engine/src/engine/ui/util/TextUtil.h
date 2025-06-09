#pragma once

#include "engine/render/Renderer.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/asset/AssetManager.h"
#include "engine/render/Material.h"
#include "platform/IWindow.h"

namespace se::ui::util
{
    template<typename T>
    concept TextTypeConcept = std::is_same_v<TextComponent, T> || std::is_same_v<EditableTextComponent, T>;

    template<TextTypeConcept T>
    void RenderText(const ecs::Id& entity,
                    const WidgetComponent& widget,
                    const RectTransformComponent& transform,
                    T& textComp,
                    const math::Vec2& windowSize,
                    render::Renderer* renderer,
                    singleton_components::UIRenderComponent* renderComp,
                    const String& text)
    {
        if (widget.visibility != Visibility::Visible || widget.parentVisibility != Visibility::Visible || text.Size() == 0)
        {
            return;
        }

        if (!textComp.materialInstance)
        {
            auto assetManager = asset::AssetManager::Get();
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/text.sass");

            static std::shared_ptr<render::Material> textMaterial = nullptr;
            if (!textMaterial)
            {
                textMaterial = render::Material::CreateMaterial({ vert }, { frag });

                render::RenderState rs;
                rs.srcBlend = render::BlendMode::SrcAlpha;
                rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
                textMaterial->SetRenderState(rs);
            }

            textComp.materialInstance = render::MaterialInstance::CreateMaterialInstance(textMaterial);
            auto texture = textComp.font->GetTextureAsset();
            textComp.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &texture);
        }

        const math::Vec2* materialPos = textComp.materialInstance->template GetUniform<math::Vec2>("pos");
        auto floatVec = math::Vec2(transform.rect.topLeft);
        if (!materialPos || *materialPos != floatVec)
        {
            textComp.materialInstance->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
        }

        if (!textComp.vertBuffer ||
            transform.lastRect.size != transform.rect.size ||
            textComp.lastText != text ||
            textComp.lastFontSize != textComp.fontSize)
        {
            auto window = Application::Get()->GetPrimaryWindow();
            asset::StaticMesh mesh = util::CreateTextMesh(transform.rect,
                                                          textComp.font,
                                                          static_cast<int>(textComp.fontSize * window->GetContentScale()),
                                                          text,
                                                          true,
                                                          textComp.wrap,
                                                          textComp.alignment);
            textComp.vertBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
            textComp.vertBuffer->CreatePlatformResource();
            textComp.indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
            textComp.indexBuffer->CreatePlatformResource();

            textComp.lastFontSize = textComp.fontSize;
            textComp.lastText = text;
        }

        textComp.materialInstance->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowSize);

        auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(textComp.materialInstance, textComp.vertBuffer, textComp.indexBuffer);

        SPARK_ASSERT(command->GetRenderStage() == render::commands::RenderStage::UI);
        renderComp->entityRenderCommands[entity].push_back(command);
    }
}
