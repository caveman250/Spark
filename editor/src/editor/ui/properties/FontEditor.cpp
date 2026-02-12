#include "FontEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Font, FontEditor, Font);

    void FontEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Font*>(value);
    }

    void FontEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "FontEditor Text");
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Name: {}\n", m_Value->GetName());
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Num Chars: {}\n", m_Value->GetNumChars());
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Ascent (at 32px): {}\n", m_Value->GetAscent(32));
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Descent (at 32px): {}\n", m_Value->GetDescent(32));
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Line Height (at 32px): {}\n", m_Value->GetLineHeight(32));
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto previewContainer = world->CreateEntity(editor->GetEditorScene(), "Preview Container");
            auto previewContainerRect = world->AddComponent<RectTransformComponent>(previewContainer);
            previewContainerRect->minHeight = static_cast<float>(std::min(m_Value->GetTextureAsset()->GetHeight(), 256u));
            world->AddComponent<WidgetComponent>(previewContainer);

            auto previewInnerContainer = world->CreateEntity(editor->GetEditorScene(), "Preview Inner Container");
            auto previewInnerContainerRect = world->AddComponent<RectTransformComponent>(previewInnerContainer);
            previewInnerContainerRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            world->AddComponent<WidgetComponent>(previewInnerContainer);
            auto image = world->AddComponent<ImageComponent>(previewInnerContainer);
            auto imageMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(imageMaterial);
            world->AddChild(previewContainer, previewInnerContainer);

            auto preview = world->CreateEntity(editor->GetEditorScene(), "Preview");
            auto previewRect = world->AddComponent<RectTransformComponent>(preview);
            previewRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            previewRect->minAspectRatio = static_cast<float>(m_Value->GetTextureAsset()->GetWidth() / m_Value->GetTextureAsset()->GetHeight());
            previewRect->maxAspectRatio = static_cast<float>(m_Value->GetTextureAsset()->GetWidth() / m_Value->GetTextureAsset()->GetHeight());
            auto previewImage = world->AddComponent<ImageComponent>(preview);
            auto alphaTexture = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
            previewImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(alphaTexture);
            previewImage->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_Value->GetTextureAsset());
            world->AddChild(previewInnerContainer, preview);

            world->AddChild(m_Content, previewContainer);
        }


    }

    void FontEditor::DestroyUI()
    {
        PropertyEditor::DestroyUI();
        for (auto editor : m_Editors)
        {
            delete editor;
        }
        m_Editors.clear();
    }

    void FontEditor::Update()
    {
        for (auto editor : m_Editors)
        {
            editor->Update();
        }
    }
}
