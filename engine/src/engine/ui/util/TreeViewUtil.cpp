#include "TreeViewUtil.h"

#include <engine/ui/components/ButtonComponent.h>

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/ImageComponent.h"

namespace se::ui::util
{
    ecs::Id CreateTreeView(TreeViewComponent** outTreeView, RectTransformComponent** outTransform, bool editorOnly)
    {
        auto world = Application::Get()->GetWorld();

        auto entity = world->CreateEntity("Tree View", editorOnly);
        *outTreeView = world->AddComponent<TreeViewComponent>(entity);
        *outTransform = world->AddComponent<RectTransformComponent>(entity);
        world->AddComponent<WidgetComponent>(entity);
        return entity;
    }

    ecs::Id InsertTreeNode(const ecs::Id& treeViewEntity,
       TreeViewComponent* treeView,
        const ecs::Id& parentNode,
        const String& name,
        TreeNodeComponent** outTreeNode,
        TextComponent** outText,
        bool editorOnly)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        treeView->dirty = true;

        auto entity = world->CreateEntity(name, editorOnly);
        world->AddChild(parentNode, entity);

        *outTreeNode = world->AddComponent<TreeNodeComponent>(entity);
        std::function<void(bool)> collapsedTreeViewCb = [treeViewEntity](bool)
        {
            auto treeView = Application::Get()->GetWorld()->GetComponent<TreeViewComponent>(treeViewEntity);
            treeView->dirty = true;
        };
        (*outTreeNode)->onCollapsedStateChange.Subscribe(std::move(collapsedTreeViewCb));

        world->AddComponent<RectTransformComponent>(entity);
        world->AddComponent<WidgetComponent>(entity);
        world->AddComponent<MouseInputComponent>(entity);

        auto textEntity = world->CreateEntity("Text", editorOnly);
        *outText = world->AddComponent<TextComponent>(textEntity);
        (*outText)->font = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        (*outText)->fontSize = 14;
        auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = 12;
        textRect->maxX = 0;
        textRect->minY = 0;
        textRect->maxY = 0;
        world->AddComponent<WidgetComponent>(textEntity);
        world->AddChild(entity, textEntity);

        static auto expanded_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_expanded.sass");
        static auto collapsed_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_collapsed.sass");

        auto statusIcon = world->CreateEntity("Status Icon", editorOnly);
        auto rect = world->AddComponent<RectTransformComponent>(statusIcon);
        rect->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 1.f };
        rect->minX = 2;
        rect->maxX = 10;
        rect->minY = 8;
        rect->maxY = 4;
        auto image = world->AddComponent<ImageComponent>(statusIcon);
        auto button = world->AddComponent<ButtonComponent>(statusIcon);
        button->image = expanded_indicator_texture;
        button->hoveredImage = expanded_indicator_texture;
        button->pressedImage = expanded_indicator_texture;
        std::function<void()> statusIconCallback = [world, entity, treeViewEntity, statusIcon]()
        {
            auto buttonComp = Application::Get()->GetWorld()->GetComponent<ButtonComponent>(statusIcon);

            auto treeView = world->GetComponent<TreeViewComponent>(treeViewEntity);
            auto treeNode = world->GetComponent<TreeNodeComponent>(entity);
            treeNode->collapsed = !treeNode->collapsed;
            const auto& texture = treeNode->collapsed ? collapsed_indicator_texture : expanded_indicator_texture;
            buttonComp->image = texture;
            buttonComp->hoveredImage = texture;
            buttonComp->pressedImage = texture;
            treeView->dirty = true;
        };
        button->onPressed.Subscribe(std::move(statusIconCallback));

        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/alpha_texture.sass");
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = render::Material::CreateMaterial({vert}, {frag});
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            material->SetRenderState(rs);
        }

        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &expanded_indicator_texture);

        world->AddComponent<MouseInputComponent>(statusIcon);

        std::function<void(bool)> collapsedImageCb = [statusIcon](bool collapsed)
        {
            auto image = Application::Get()->GetWorld()->GetComponent<ImageComponent>(statusIcon);
            if (collapsed)
            {
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &collapsed_indicator_texture);
            }
            else
            {
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &expanded_indicator_texture);
            }
        };
        (*outTreeNode)->onCollapsedStateChange.Subscribe(std::move(collapsedImageCb));

        world->AddChild(entity, statusIcon);

        return entity;
    }
}
