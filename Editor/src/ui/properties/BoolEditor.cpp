#include "BoolEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(bool, BoolEditor);

    DEFINE_SPARK_CLASS_BEGIN(BoolEditor)
    DEFINE_SPARK_CLASS_END()

    void BoolEditor::SetValue(void* value)
    {
        m_Value = static_cast<bool*>(value);
    }

    void BoolEditor::ConstructUI(const String& name, bool constructTitle)
    {
       PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("Bool Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = 0.5f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = constructTitle ?  0 : 22;
        bgTransform->maxY = constructTitle ?  32 : 52;
        bgTransform->maxX = 30;
        world->AddComponent<WidgetComponent>(bg);
        world->AddChild(m_WidgetId, bg);

        m_RectTransform->maxY = bgTransform->maxY + 2;

        auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/alpha_texture.sass");

        m_Tickbox = world->CreateEntity("Border", true);
        auto innerImage = world->AddComponent<ImageComponent>(m_Tickbox);
        innerImage->material = render::Material::CreateMaterial({vert}, {frag});
        m_CheckedTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/checkbox_checked.sass");
        m_UncheckedTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/checkbox_unchecked.sass");
        innerImage->material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_UncheckedTexture);
        render::RenderState rs;
        rs.srcBlend = render::BlendMode::SrcAlpha;
        rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
        innerImage->material->SetRenderState(rs);
        auto innerTransform = world->AddComponent<RectTransformComponent>(m_Tickbox);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minY = innerTransform->maxY = 2.f;
        world->AddChild(bg, m_Tickbox);
    }

    void BoolEditor::Update()
    {
        if (m_Value && m_LastValue != *m_Value)
        {
            auto tick = Application::Get()->GetWorld()->GetComponent<ImageComponent>(m_Tickbox);
            tick->material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1,
                *m_Value == true ? &m_CheckedTexture : &m_UncheckedTexture);
            m_LastValue = *m_Value;
        }
    }
}
