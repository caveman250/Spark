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
    concept TextComponentType = std::is_same_v<TextComponent, T> || std::is_same_v<EditableTextComponent, T>;

    template <TextComponentType T>
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

        bool invalidate = transform.lastRect.size != transform.rect.size ||
            textComp.lastText != text ||
            textComp.lastFontSize != textComp.fontSize;

        if (!textComp.vertBuffer || invalidate)
        {
            const auto window = Application::Get()->GetWindow();
            const TextMeshParams params = {
                .rect = &transform.rect,
                .font = textComp.font.GetAsset().get(),
                .fontSize = static_cast<int>(textComp.fontSize * window->GetContentScale()),
                .text = &text,
                .applyKerning = true,
                .wrap = textComp.wrap,
                .justification = textComp.alignment
            };
            const asset::StaticMesh mesh = util::CreateTextMesh(params);
            textComp.vertBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
            textComp.vertBuffer->CreatePlatformResource();
            textComp.indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
            textComp.indexBuffer->CreatePlatformResource();
        }

        if constexpr (std::is_same_v<T, EditableTextComponent>)
        {
            bool invalidateSelection = textComp.selectionStart != textComp.lastSelectionStart ||
                                        textComp.selectionEnd != textComp.lastSelectionEnd;
            if ((!textComp.selectionVertBuffer || invalidate || invalidateSelection) &&
                textComp.inEditMode &&
                textComp.selectionStart != -1 &&
                textComp.selectionEnd != -1)
            {
                const asset::StaticMesh selectionMesh = util::CreateTextSelectionMesh(textComp, transform);
                textComp.selectionVertBuffer = render::VertexBuffer::CreateVertexBuffer(selectionMesh);
                textComp.selectionVertBuffer->CreatePlatformResource();
                textComp.selectionIndexBuffer = render::IndexBuffer::CreateIndexBuffer(selectionMesh);
                textComp.selectionIndexBuffer->CreatePlatformResource();

                textComp.lastSelectionStart = textComp.selectionStart;
                textComp.lastSelectionEnd = textComp.selectionEnd;
            }

            if (!textComp.selectionMaterialInstance && textComp.inEditMode)
            {
                auto textMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editable_text_selection.sass");
                textComp.selectionMaterialInstance = render::MaterialInstance::CreateMaterialInstance(textMaterial);
            }
        }

        if (invalidate)
        {
            textComp.lastFontSize = textComp.fontSize;
            textComp.lastText = text;
        }
    }

    template <TextComponentType T>
    void RenderText(const ecs::Id& entity,
                    const WidgetComponent& widget,
                    const RectTransformComponent& transform,
                    T& textComp,
                    const math::Vec2& windowSize,
                    render::Renderer* renderer,
                    singleton_components::UIRenderComponent* renderComp,
                    const std::string& text,
                    const math::Vec2& renderOffset)
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
        auto floatVec = math::Vec2(transform.rect.topLeft) + renderOffset;
        if (!materialPos || *materialPos != floatVec)
        {
            textComp.materialInstance->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &floatVec);
        }

        textComp.materialInstance->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowSize);

#if SPARK_EDITOR
        const auto editor = Application::Get()->GetEditorRuntime();
        const bool isEditorEntity = *entity.scene == editor->GetEditorScene();
#else
        constexpr bool isEditorEntity = false;
#endif
        renderComp->mutex.lock();
        if constexpr (std::is_same_v<T, EditableTextComponent>)
        {
            if (textComp.inEditMode)
            {
                const auto pushScissor = renderer->AllocRenderCommand<render::commands::PushScissor>(transform.rect);
                renderComp->entityPreRenderCommands[entity].push_back(UIRenderCommand(pushScissor, UILayerKey(transform.layer, isEditorEntity)));

                if (textComp.selectionMaterialInstance && textComp.selectionStart != -1 && textComp.selectionEnd != -1)
                {
                    const math::Vec2* selectionMaterialPos = textComp.selectionMaterialInstance->template GetUniform<math::Vec2>("pos");
                    auto selectionFloatVec = math::Vec2(transform.rect.topLeft) + renderOffset;
                    if (!selectionMaterialPos || *selectionMaterialPos != selectionFloatVec)
                    {
                        textComp.selectionMaterialInstance->SetUniform("pos", asset::shader::ast::AstType::Vec2, 1, &selectionFloatVec);
                    }
                    textComp.selectionMaterialInstance->SetUniform("screenSize", asset::shader::ast::AstType::Vec2, 1, &windowSize);

                    auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(textComp.selectionMaterialInstance,
                        textComp.selectionVertBuffer,
                        textComp.selectionIndexBuffer);
                    renderComp->entityRenderCommands[entity].push_back(UIRenderCommand(command, UILayerKey(transform.layer, isEditorEntity)));
                }
            }
        }
        auto command = renderer->AllocRenderCommand<render::commands::SubmitUI>(textComp.materialInstance, textComp.vertBuffer, textComp.indexBuffer);
        SPARK_ASSERT(command->GetRenderStage() == render::commands::RenderStage::UI);
        renderComp->entityRenderCommands[entity].push_back(UIRenderCommand(command, UILayerKey(transform.layer, isEditorEntity)));
        if constexpr (std::is_same_v<T, EditableTextComponent>)
        {
            if (textComp.inEditMode)
            {
                const auto popScissor = renderer->AllocRenderCommand<render::commands::PopScissor>();
                renderComp->entityPostRenderCommands[entity].push_back(UIRenderCommand(popScissor, UILayerKey(transform.layer, isEditorEntity)));
            }
        }
        renderComp->mutex.unlock();
    }
}
