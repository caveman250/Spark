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
        m_TestTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button.sass");
        for (size_t i = 0; i < 10; ++i)
        {
            ecs::Id imageEntity = world->CreateEntity("Image", true);
            auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(imageEntity);
            rect->maxX = 100;
            rect->maxY = 100;
            auto image = world->AddComponent<se::ui::components::ImageComponent>(imageEntity);

            static std::shared_ptr<render::Material> material = nullptr;
            if (!material)
            {
                auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
                auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/diffuse_texture.sass");

                material = render::Material::CreateMaterial({vert}, {frag});
            }

            image->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(material);
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_TestTexture);

            world->AddChild(gridBoxEntity, imageEntity);
        }

        gridBox->dirty = true;
    }

    void AssetBrowserWindow::DestroyUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        world->DestroyEntity(m_Window);
        m_Window = ecs::s_InvalidEntity;
    }
}