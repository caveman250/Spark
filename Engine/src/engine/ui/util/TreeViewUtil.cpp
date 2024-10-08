#include "TreeViewUtil.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/ImageComponent.h"

namespace se::ui::util
{
    ecs::Id CreateTreeView(components::TreeViewComponent** outTreeView, components::RectTransformComponent** outTransform)
    {
        auto world = Application::Get()->GetWorld();

        auto entity = world->CreateEntity();
        *outTreeView = world->AddComponent<components::TreeViewComponent>(entity);
        *outTransform = world->AddComponent<components::RectTransformComponent>(entity);
        return entity;
    }

    ecs::Id InsertTreeNode(ecs::Id treeViewEntity,
       components::TreeViewComponent* treeView,
        ecs::Id parentNode,
        components::TreeNodeComponent** outTreeNode,
        components::TextComponent** outText)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        treeView->dirty = true;

        auto entity = world->CreateEntity();
        world->AddChild(parentNode, entity);

        *outTreeNode = world->AddComponent<components::TreeNodeComponent>(entity);
        (*outTreeNode)->onCollapsedStateChange.Subscribe<components::TreeViewComponent>(treeViewEntity, [](bool, components::TreeViewComponent* treeView)
        {
            treeView->dirty = true;
        });

        world->AddComponent<components::RectTransformComponent>(entity);
        world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
        world->AddComponent<components::WidgetComponent>(entity);

        auto textEntity = world->CreateEntity();
        *outText = world->AddComponent<components::TextComponent>(textEntity);
        (*outText)->font = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
        (*outText)->fontSize = 18;
        auto textRect = world->AddComponent<components::RectTransformComponent>(textEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = 12;
        textRect->maxX = 0;
        textRect->minY = 0;
        textRect->maxY = 0;
        world->AddComponent<components::WidgetComponent>(textEntity);
        world->AddChild(entity, textEntity);


        // TODO make this better.
        static auto expanded_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/builtin_assets/textures/tree_node_indicator_expanded.sass");
        static auto collapsed_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/builtin_assets/textures/tree_node_indicator_collapsed.sass");

        auto statusIcon = world->CreateEntity();
        auto rect = world->AddComponent<components::RectTransformComponent>(statusIcon);
        rect->anchors = { .left = 0.f, .right = 0.f, .top = 0.75f, .bottom = 0.75f };
        rect->minX = 2;
        rect->maxX = 10;
        rect->minY = -4;
        rect->maxY = -4;
        auto image = world->AddComponent<components::ImageComponent>(statusIcon);
        auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/alpha_texture.sass");
        image->material = render::Material::CreateMaterial({vert}, {frag});
        image->material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &expanded_indicator_texture);
        auto rs = render::RenderState();
        rs.srcBlend = render::BlendMode::SrcAlpha;
        rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
        image->material->SetRenderState(rs);

        (*outTreeNode)->onCollapsedStateChange.Subscribe<components::ImageComponent>(statusIcon, [](bool collapsed, components::ImageComponent* image)
        {
            if (collapsed)
            {
                image->material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &collapsed_indicator_texture);
            }
            else
            {
                image->material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &expanded_indicator_texture);
            }
        });

        world->AddChild(entity, statusIcon);

        return entity;
    }
}
