#include "TreeViewUtil.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/shader/Shader.h"
#include "engine/render/Material.h"
#include "engine/render/RenderState.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    ecs::Id CreateTreeView(TreeViewComponent** outTreeView, RectTransformComponent** outTransform, const ecs::Id& scene)
    {
        auto world = Application::Get()->GetWorld();

        auto entity = world->CreateEntity(scene, "Tree View");
        *outTreeView = world->AddComponent<TreeViewComponent>(entity);
        *outTransform = world->AddComponent<RectTransformComponent>(entity);
        world->AddComponent<WidgetComponent>(entity);
        return entity;
    }

    NewTreeNode InsertTreeNode(const TreeNodeParams& params)
    {
        NewTreeNode ret = {};
        auto world = Application::Get()->GetWorld();

        params.treeViewRect->needsLayout = true;

        ret.entity = world->CreateEntity(params.scene, params.name);
        world->AddChild(params.parentNode, ret.entity);

        ret.treeNode = world->AddComponent<TreeNodeComponent>(ret.entity);
        ret.treeNode->contextOptions = params.contextOptions;
        ret.treeNode->onContextMenuOptionSelected = params.onContextMenuOptionSelected;
        std::function<void(bool)> collapsedTreeViewCb = [params](bool)
        {
            auto treeView = Application::Get()->GetWorld()->GetComponent<TreeViewComponent>(params.treeViewEntity);
            treeView->dirty = true;
        };
        ret.treeNode->onCollapsedStateChange.Subscribe(std::move(collapsedTreeViewCb));

        world->AddComponent<RectTransformComponent>(ret.entity);
        world->AddComponent<WidgetComponent>(ret.entity);
        world->AddComponent<MouseInputComponent>(ret.entity);

        auto textEntity = world->CreateEntity(params.scene, "Text");
        ret.text = world->AddComponent<TextComponent>(textEntity);
        ret.text->font = "/engine_assets/fonts/Arial.sass";
        ret.text->fontSize = 14;
        auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = 12;
        textRect->maxX = 0;
        textRect->minY = 0;
        textRect->maxY = 0;
        world->AddComponent<WidgetComponent>(textEntity);
        world->AddChild(ret.entity, textEntity);

        static asset::AssetReference<asset::Texture> expanded_indicator_texture = "/engine_assets/textures/tree_node_indicator_expanded.sass";
        static asset::AssetReference<asset::Texture> collapsed_indicator_texture = "/engine_assets/textures/tree_node_indicator_collapsed.sass";

        auto statusIcon = world->CreateEntity(params.scene, "Status Icon");
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
        std::function statusIconCallback = [world, entity = ret.entity, params, statusIcon](input::MouseButton)
        {
            auto buttonComp = Application::Get()->GetWorld()->GetComponent<ButtonComponent>(statusIcon);

            auto treeView = world->GetComponent<TreeViewComponent>(params.treeViewEntity);
            auto treeNode = world->GetComponent<TreeNodeComponent>(entity);
            treeNode->collapsed = !treeNode->collapsed;
            const auto& texture = treeNode->collapsed ? collapsed_indicator_texture : expanded_indicator_texture;
            buttonComp->image = texture;
            buttonComp->hoveredImage = texture;
            buttonComp->pressedImage = texture;
            treeView->dirty = true;
        };
        button->onPressed.Subscribe(std::move(statusIconCallback));

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

        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &expanded_indicator_texture);

        world->AddComponent<MouseInputComponent>(statusIcon);

        std::function<void(bool)> collapsedImageCb = [statusIcon](bool collapsed)
        {
            auto image = Application::Get()->GetWorld()->GetComponent<ImageComponent>(statusIcon);
            if (collapsed)
            {
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &collapsed_indicator_texture);
            }
            else
            {
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &expanded_indicator_texture);
            }
        };
        ret.treeNode->onCollapsedStateChange.Subscribe(std::move(collapsedImageCb));

        world->AddChild(ret.entity, statusIcon);

        return ret;
    }
}
