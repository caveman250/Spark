#include "CollapsingHeaderUtil.h"

#include "Widgets.generated.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    ecs::Id CreateCollapsingHeader(ecs::World* world,
                                   ecs::Id& titleContainer,
                                   ecs::Id& contents,
                                   CollapsingHeaderComponent** collapsingHeader,
                                   bool collapsed,
                                   bool withBackground,
                                   bool editorOnly)
    {
        auto assetManager = asset::AssetManager::Get();

        auto entity = world->CreateEntity("Collapsing Header", editorOnly);
        auto rect = world->AddComponent<RectTransformComponent>(entity);
        rect->overridesChildSizes = true;
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        *collapsingHeader = world->AddComponent<CollapsingHeaderComponent>(entity);
        (*collapsingHeader)->collapsed = collapsed;
        world->AddComponent<MouseInputComponent>(entity);
        world->AddComponent<WidgetComponent>(entity);

        static std::shared_ptr<render::Material> titleBGMaterial = nullptr;
        if (!titleBGMaterial)
        {
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
            titleBGMaterial = render::Material::CreateMaterial({ vert }, { frag });
            titleBGMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(.24f, .24f, .24f));
        }

        ecs::Id title = world->CreateEntity("Collapsing Header Title", editorOnly);
        rect = world->AddComponent<RectTransformComponent>(title);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(title);
        if (withBackground)
        {
            auto titleImage = world->AddComponent<ImageComponent>(title);
            titleImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(titleBGMaterial);
        }
        world->AddChild(entity, title);
        (*collapsingHeader)->titleEntity = title;

        titleContainer = world->CreateEntity("Collapsing Header Title Contents", editorOnly);
        rect = world->AddComponent<RectTransformComponent>(titleContainer);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 12;
        world->AddComponent<WidgetComponent>(titleContainer);
        world->AddChild(title, titleContainer);

        static auto expanded_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_expanded.sass");
        static auto collapsed_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_collapsed.sass");

        ecs::Id titleIndicator = world->CreateEntity("Collapsing Header Title Indicator", editorOnly);
        rect = world->AddComponent<RectTransformComponent>(titleIndicator);
        rect->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 0;
        rect->maxX = 8;
        rect->minY = 4;
        rect->maxY = 12;
        world->AddComponent<WidgetComponent>(titleIndicator);
        auto indicatorImage = world->AddComponent<ImageComponent>(titleIndicator);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/alpha_texture.sass");
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = render::Material::CreateMaterial({ vert }, { frag });
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            material->SetRenderState(rs);
        }
        indicatorImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        indicatorImage->materialInstance->SetUniform("Texture",
                                                     asset::shader::ast::AstType::Sampler2D,
                                                     1,
                                                     collapsed ? &collapsed_indicator_texture : &expanded_indicator_texture);
        world->AddChild(title, titleIndicator);

        auto titleButtonEntity = world->CreateEntity("Collapsing Header Title Button", editorOnly);
        ButtonComponent* titleButton = world->AddComponent<ButtonComponent>(titleButtonEntity);
        RectTransformComponent* titleButtonRect = world->AddComponent<RectTransformComponent>(titleButtonEntity);
        titleButtonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto titleButtonWidget = world->AddComponent<WidgetComponent>(titleButtonEntity);
        titleButtonWidget->visibility = Visibility::Hidden;
        world->AddChild(title, titleButtonEntity);
        titleButton->onReleased.Subscribe([world, entity, titleIndicator]()
        {
            CollapsingHeaderComponent* collapsingHeader = world->GetComponent<CollapsingHeaderComponent>(entity);
            collapsingHeader->collapsed = !collapsingHeader->collapsed;

            ImageComponent* image = world->GetComponent<ImageComponent>(titleIndicator);
            image->materialInstance->SetUniform("Texture",
                                                asset::shader::ast::AstType::Sampler2D,
                                                1,
                                                collapsingHeader->collapsed ? &collapsed_indicator_texture : &expanded_indicator_texture);
        });

        contents = world->CreateEntity("Collapsing Header Contents", editorOnly);
        rect = world->AddComponent<RectTransformComponent>(contents);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(contents);
        world->AddChild(entity, contents);
        (*collapsingHeader)->contentsEntity = contents;

        return entity;
    }
}
