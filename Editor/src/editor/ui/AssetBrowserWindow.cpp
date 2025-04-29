#include <engine/ui/components/TextComponent.h>
#include "AssetBrowserWindow.h"
#include "engine/ui/components/GridBoxComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/Application.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/WindowUtil.h"
#include "engine/render/FrameBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/asset/AssetManager.h"

namespace se::editor::ui
{
    void AssetBrowserWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        se::ui::components::RectTransformComponent *windowTransform;
        se::ui::components::WindowComponent *windowComp;
        se::ui::components::TitleBarComponent *titleBarComp;
        ecs::Id contentArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
                                                &windowComp,
                                                &titleBarComp,
                                                contentArea,
                                                "Asset Browser",
                                                [this]()
                                                { DestroyUI(); },
                                                true);
        windowTransform->anchors = {0.f, 0.f, 0.f, 0.f};
        windowTransform->minX = 20;
        windowTransform->maxX = 850;
        windowTransform->minY = 390;
        windowTransform->maxY = 700;

        ecs::Id gridBoxEntity = world->CreateEntity("Grid Box", true);
        auto* gridRect = world->AddComponent<se::ui::components::RectTransformComponent>(gridBoxEntity);
        gridRect->overridesChildSizes = true;
        gridRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        gridRect->minX = 5;
        gridRect->minY = 5;
        gridRect->maxX = 5;
        gridRect->maxY = 5;
        auto* gridBox = world->AddComponent<se::ui::components::GridBoxComponent>(gridBoxEntity);
        gridBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(gridBoxEntity);
        world->AddChild(contentArea, gridBoxEntity);

        auto assetManager = se::asset::AssetManager::Get();
        m_TestTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_file.sass");
        auto arial = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
        for (size_t i = 0; i < 10; ++i)
        {
            ecs::Id fileEntity = world->CreateEntity("File", true);
            auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(fileEntity);
            rect->maxX = 100;
            rect->maxY = 100;

            ecs::Id imageEntity = world->CreateEntity("Image", true);
            auto imageRect = world->AddComponent<se::ui::components::RectTransformComponent>(imageEntity);
            imageRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            imageRect->maxY = 20;
            imageRect->minAspectRatio = 1.f;
            imageRect->maxAspectRatio = 1.f;
            auto image = world->AddComponent<se::ui::components::ImageComponent>(imageEntity);
            static std::shared_ptr<render::Material> material = nullptr;
            if (!material)
            {
                auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
                auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/alpha_texture.sass");

                material = render::Material::CreateMaterial({vert}, {frag});
                auto rs = render::RenderState();
                rs.srcBlend = render::BlendMode::SrcAlpha;
                rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
                material->SetRenderState(rs);
            }
            image->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(material);
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_TestTexture);
            world->AddChild(fileEntity, imageEntity);

            auto textEntity = world->CreateEntity("Text", true);
            auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
            text->font = arial;
            text->fontSize = 16;
            text->text = "File";
            text->justification = se::ui::text::Alignment::Center;
            auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
            textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
            textRect->minY = 20;
            world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
            world->AddChild(fileEntity, textEntity);

            world->AddChild(gridBoxEntity, fileEntity);
        }

        gridBox->dirty = true;
    }

    void AssetBrowserWindow::DestroyUI()
    {

    }
}