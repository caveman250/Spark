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
    void UpdateText(T& textComp, const RectTransformComponent& transform, const std::string& text)
    {
        if (!textComp.materialInstance && textComp.font.IsSet())
        {
            auto textMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/text.sass");
            textComp.materialInstance = render::MaterialInstance::CreateMaterialInstance(textMaterial);
            auto texture = textComp.font.GetAsset()->GetTextureAsset();
            textComp.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &texture);
            float smoothing = textComp.fontSize > 50 ? 0.01f : 0.5f;
            textComp.materialInstance->SetUniform("smoothing", asset::shader::ast::AstType::Float, 1, &smoothing);
        }

        if (!textComp.vertBuffer ||
            transform.lastRect.size != transform.rect.size ||
            textComp.lastText != text ||
            textComp.lastFontSize != textComp.fontSize)
        {
            auto window = Application::Get()->GetWindow();
            asset::StaticMesh mesh = util::CreateTextMesh(transform.rect,
                                                          textComp.font.GetAsset(),
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
    }

    template<TextTypeConcept T>
    void RenderText(const ecs::Id& entity,
                    const WidgetComponent& widget,
                    const RectTransformComponent& transform,
                    T& textComp,
                    const math::Vec2& windowSize,
                    render::Renderer* renderer,
                    singleton_components::UIRenderComponent* renderComp,
                    const std::string& text)
    {
        if (widget.visibility != Visibility::Visible || widget.parentVisibility != Visibility::Visible || text.size() == 0)
        {
            return;
        }

        if (!textComp.materialInstance || !textComp.vertBuffer)
        {
            return;
        }

        const math::Vec2* materialPos = textComp.materialInstance->template GetUniform<math::Vec2>("pos");
        auto floatVec = math::Vec2(transform.rect.topLeft);
        if (!materialPos || *materialPos != floatVec)
        {
            textComp.materialInstance->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
        }

        textComp.materialInstance->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowSize);

        auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(textComp.materialInstance, textComp.vertBuffer, textComp.indexBuffer);

        SPARK_ASSERT(command->GetRenderStage() == render::commands::RenderStage::UI);
#if SPARK_EDITOR
        auto editor = Application::Get()->GetEditorRuntime();
        bool isEditorEntity = *entity.scene == editor->GetEditorScene();
#else
        constexpr bool isEditorEntity = false;
#endif
        renderComp->mutex.lock();
        renderComp->entityRenderCommands[entity].push_back(UIRenderCommand(command, UILayerKey(transform.layer, isEditorEntity)));
        renderComp->mutex.unlock();
    }
}
