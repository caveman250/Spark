#include "CollapsingHeaderUtil.h"

#include "Widgets.generated.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    ecs::Id CreateCollapsingHeader(ecs::World* world,
                                   const std::string& name,
                                   ecs::Id& titleContainer,
                                   ecs::Id& contents,
                                   CollapsingHeaderComponent** collapsingHeader,
                                   bool collapsed,
                                   bool withBackground,
                                   const ecs::Id& scene)
    {
        auto assetManager = asset::AssetManager::Get();

        auto entity = world->CreateEntity(scene, name);
        auto rect = world->AddComponent<RectTransformComponent>(entity);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        *collapsingHeader = world->AddComponent<CollapsingHeaderComponent>(entity);
        (*collapsingHeader)->collapsed = collapsed;
        world->AddComponent<MouseInputComponent>(entity);
        world->AddComponent<WidgetComponent>(entity);

        ecs::Id title = world->CreateEntity(scene, std::format("Collapsing Header Title ({})", name));
        rect = world->AddComponent<RectTransformComponent>(title);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(title);
        if (withBackground)
        {
            auto titleImage = world->AddComponent<ImageComponent>(title);
            auto titleBgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_collapsing_header_title.sass");
            titleImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(titleBgMaterial);
        }
        world->AddChild(entity, title);
        (*collapsingHeader)->titleEntity = title;

        titleContainer = world->CreateEntity(scene, std::format("Collapsing Header Title Contents ({})", name));
        rect = world->AddComponent<RectTransformComponent>(titleContainer);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 12;
        world->AddComponent<WidgetComponent>(titleContainer);
        world->AddChild(title, titleContainer);

        static asset::AssetReference<asset::Texture> expanded_indicator_texture = "/engine_assets/textures/tree_node_indicator_expanded.sass";
        static asset::AssetReference<asset::Texture> collapsed_indicator_texture = "/engine_assets/textures/tree_node_indicator_collapsed.sass";

        ecs::Id titleIndicator = world->CreateEntity(scene, std::format("Collapsing Header Title Indicator ({})", name));
        rect = world->AddComponent<RectTransformComponent>(titleIndicator);
        rect->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 0;
        rect->maxX = 8;
        rect->minY = 4;
        rect->maxY = 12;
        world->AddComponent<WidgetComponent>(titleIndicator);
        auto indicatorImage = world->AddComponent<ImageComponent>(titleIndicator);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/alpha_texture.sass") });
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            material->SetRenderState(rs);
        }
        indicatorImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        indicatorImage->materialInstance->SetUniform("Texture",
                                                     asset::shader::ast::AstType::Sampler2DReference,
                                                     1,
                                                     collapsed ? &collapsed_indicator_texture : &expanded_indicator_texture);
        world->AddChild(title, titleIndicator);

        auto titleButtonEntity = world->CreateEntity(scene, std::format("Collapsing Header Title Button ({})", name));
        components::ButtonComponent* titleButton = world->AddComponent<components::ButtonComponent>(titleButtonEntity);
        components::RectTransformComponent* titleButtonRect = world->AddComponent<components::RectTransformComponent>(titleButtonEntity);
        titleButtonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto titleButtonWidget = world->AddComponent<components::WidgetComponent>(titleButtonEntity);
        titleButtonWidget->visibility = Visibility::Hidden;
        world->AddChild(title, titleButtonEntity);
        titleButton->onReleased.Subscribe([world, entity, titleIndicator]()
        {
            components::CollapsingHeaderComponent* collapsingHeader = world->GetComponent<components::CollapsingHeaderComponent>(entity);
            collapsingHeader->collapsed = !collapsingHeader->collapsed;

            components::ImageComponent* image = world->GetComponent<components::ImageComponent>(titleIndicator);
            image->materialInstance->SetUniform("Texture",
                                                asset::shader::ast::AstType::Sampler2DReference,
                                                1,
                                                collapsingHeader->collapsed ? &collapsed_indicator_texture : &expanded_indicator_texture);
        });

        contents = world->CreateEntity(scene, std::format("Collapsing Header Contents ({})", name));
        rect = world->AddComponent<components::RectTransformComponent>(contents);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<components::WidgetComponent>(contents);
        world->AddChild(entity, contents);
        (*collapsingHeader)->contentsEntity = contents;

        return entity;
    }
}
