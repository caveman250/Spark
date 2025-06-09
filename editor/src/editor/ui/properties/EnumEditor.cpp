#include "EnumEditor.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::editor::ui::properties
{
    void EnumEditor::SetValue(void* value, const reflect::Type* type)
    {
        m_Value = static_cast<int*>(value);
        m_EnumType = static_cast<const reflect::Enum*>(type);
    }

    void EnumEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
       PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        constexpr int fontSize = 14;
        constexpr int padding = 4;
        constexpr int textYOffset = padding / 2;
        constexpr int borderSize = 2;

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto entity = world->CreateEntity("Enum Editor", true);
        auto rectTransform = world->AddComponent<RectTransformComponent>(entity);
        rectTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        rectTransform->minY = 0;
        rectTransform->maxY = static_cast<int>(ariel->GetLineHeight(fontSize) + padding + borderSize * 2 + 0.5f);
        world->AddComponent<WidgetComponent>(entity);
        world->AddChild(m_Content, entity);

        auto bg = world->CreateEntity("Enum Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };

        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto flat_color = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto innerImageEntity = world->CreateEntity("Border", true);
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(bg, innerImageEntity);

        m_ComboBox = world->CreateEntity("Label", true);
        auto text = world->AddComponent<EditableTextComponent>(m_ComboBox);
        text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = fontSize;
        text->text = std::format("{}", m_EnumType->ToString(*m_Value));
        auto labelRect = world->AddComponent<RectTransformComponent>(m_ComboBox);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minX = 2;
        labelRect->maxX = 2;
        labelRect->minY = textYOffset;
        world->AddChild(innerImageEntity, m_ComboBox);

        ecs::Id titleIndicator = world->CreateEntity("Collapsing Header Title Indicator", true);
        auto indicatorRect = world->AddComponent<RectTransformComponent>(titleIndicator);
        indicatorRect->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        indicatorRect->minX = 12;
        indicatorRect->maxX = 4;
        indicatorRect->minY = 4;
        indicatorRect->maxY = 12;
        world->AddComponent<WidgetComponent>(titleIndicator);
        auto indicatorImage = world->AddComponent<ImageComponent>(titleIndicator);
        auto alpha_texture = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/alpha_texture.sass");
        static std::shared_ptr<render::Material> indicatorMaterial = nullptr;
        if (!indicatorMaterial)
        {
            indicatorMaterial = render::Material::CreateMaterial({ vert }, { alpha_texture });
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            indicatorMaterial->SetRenderState(rs);
        }
        static auto expanded_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_expanded.sass");
        indicatorImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(indicatorMaterial);
        indicatorImage->materialInstance->SetUniform("Texture",
                                                     asset::shader::ast::AstType::Sampler2D,
                                                     1,
                                                     &expanded_indicator_texture);
        world->AddChild(innerImageEntity, titleIndicator);

        world->AddChild(entity, bg);
    }

    void EnumEditor::Update()
    {
        if (m_Value && m_LastValue != *m_Value)
        {
            // const auto& texture = *m_Value == true ? m_CheckedTexture : m_UncheckedTexture;
            // if (auto button = Application::Get()->GetWorld()->GetComponent<ButtonComponent>(m_Tickbox))
            // {
            //     button->image = texture;
            //     button->pressedImage = texture;
            //     button->hoveredImage = texture;
            // }
            //
            // if (auto image = Application::Get()->GetWorld()->GetComponent<ImageComponent>(m_Tickbox))
            // {
            //     image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &texture);
            // }
            // m_LastValue = *m_Value;
        }
    }
}
