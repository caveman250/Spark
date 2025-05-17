#include "FontEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/asset/shader/ast/MainNode.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/util/VerticalBoxUtil.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Font, FontEditor, Font);

    DEFINE_SPARK_CLASS_BEGIN(FontEditor)
    DEFINE_SPARK_CLASS_END(FontEditor)

    void FontEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Font*>(value);
    }

    void FontEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors);

        auto world = Application::Get()->GetWorld();

        {
            auto textEntity = world->CreateEntity("FontEditor Text", true);
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Name: {}\n", m_Value->GetName());
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        auto reflect = reflect::ClassResolver<asset::Font>::get();
        if (auto *propEditor = properties::CreatePropertyEditor(*reflect->GetMember("m_Texture"), m_Value, {0.f, 1.f, 0.f, 0.f}, true))
        {
            world->AddChild(m_Content, propEditor->GetWidgetId());
            m_Editors.push_back(propEditor);
        }

        {
            auto textEntity = world->CreateEntity("Text", true);
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Num Chars: {}\n", m_Value->GetNumChars());
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity("Text", true);
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Ascent (at 32px): {}\n", m_Value->GetAscent(32));
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity("Text", true);
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Descent (at 32px): {}\n", m_Value->GetDescent(32));
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
        }

        {
            auto textEntity = world->CreateEntity("Text", true);
            auto* textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->text = std::format("Line Height (at 32px): {}\n", m_Value->GetLineHeight(32));
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            world->AddChild(m_Content, textEntity);
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
