#include "BoolEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>

#include "engine/ui/components/ButtonComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(bool, BoolEditor, bool);

    void BoolEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<bool*>(value);
    }

    void BoolEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
       PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("Bool Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 0.35f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = 24;
        bgTransform->maxX = 24;
        world->AddComponent<WidgetComponent>(bg);
        world->AddChild(m_Content, bg);

        m_RectTransform->maxY = bgTransform->maxY + 2;
        m_Tickbox = world->CreateEntity("Border", true);
        auto button = world->AddComponent<ButtonComponent>(m_Tickbox);
        m_CheckedTexture = assetManager->GetAsset<asset::Texture>("/engine_assets/textures/checkbox_checked.sass");
        m_UncheckedTexture = assetManager->GetAsset<asset::Texture>("/engine_assets/textures/checkbox_unchecked.sass");
        button->image = *m_Value == true ? m_CheckedTexture : m_UncheckedTexture;
        button->pressedImage = *m_Value == true ? m_CheckedTexture : m_UncheckedTexture;
        button->hoveredImage = *m_Value == true ? m_CheckedTexture : m_UncheckedTexture;
        button->onReleased.Subscribe([this]()
        {
            *m_Value = !(*m_Value);
        });
        auto innerTransform = world->AddComponent<RectTransformComponent>(m_Tickbox);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddChild(bg, m_Tickbox);
    }

    void BoolEditor::Update()
    {
        if (m_Value && m_LastValue != *m_Value)
        {
            const auto& texture = *m_Value == true ? m_CheckedTexture : m_UncheckedTexture;
            if (auto button = Application::Get()->GetWorld()->GetComponent<ButtonComponent>(m_Tickbox))
            {
                button->image = texture;
                button->pressedImage = texture;
                button->hoveredImage = texture;
            }

            if (auto image = Application::Get()->GetWorld()->GetComponent<ImageComponent>(m_Tickbox))
            {
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &texture);
            }
            m_LastValue = *m_Value;
        }
    }
}
