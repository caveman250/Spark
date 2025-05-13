#include "BoolEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(bool, BoolEditor, bool);

    DEFINE_SPARK_CLASS_BEGIN(BoolEditor)
    DEFINE_SPARK_CLASS_END(BoolEditor)

    void BoolEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<bool*>(value);
    }

    void BoolEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
       PropertyEditor::ConstructUI(name, constructTitle, anchors);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("Bool Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.3f : 0.f, .right = 0.3f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = 32;
        bgTransform->maxX = 30;
        world->AddComponent<WidgetComponent>(bg);
        world->AddChild(m_WidgetId, bg);

        m_RectTransform->maxY = bgTransform->maxY + 2;
        m_Tickbox = world->CreateEntity("Border", true);
        auto innerImage = world->AddComponent<ImageComponent>(m_Tickbox);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/alpha_texture.sass");

            material = render::Material::CreateMaterial({vert}, {frag});
            render::RenderState rs;
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            material->SetRenderState(rs);
        }

        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        m_CheckedTexture = assetManager->GetAsset<asset::Texture>("/engine_assets/textures/checkbox_checked.sass");
        m_UncheckedTexture = assetManager->GetAsset<asset::Texture>("/engine_assets/textures/checkbox_unchecked.sass");
        innerImage->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_UncheckedTexture);

        auto innerTransform = world->AddComponent<RectTransformComponent>(m_Tickbox);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minY = innerTransform->maxY = 2;
        world->AddChild(bg, m_Tickbox);
    }

    void BoolEditor::Update()
    {
        if (m_Value && m_LastValue != *m_Value)
        {
            auto tick = Application::Get()->GetWorld()->GetComponent<ImageComponent>(m_Tickbox);
            tick->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1,
                *m_Value == true ? &m_CheckedTexture : &m_UncheckedTexture);
            m_LastValue = *m_Value;
        }
    }
}
