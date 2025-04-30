#include "AssetBrowserWindow.h"
#include "engine/ui/components/GridBoxComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/Application.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/util/WindowUtil.h"
#include "engine/render/FrameBuffer.h"
#include "engine/render/MaterialInstance.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/util/VerticalBoxUtil.h"

namespace se::editor::ui
{
    void AssetBrowserWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto assetManager = se::asset::AssetManager::Get();

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

        ecs::Id verticalBoxEntity = world->CreateEntity("AssetBrowser VerticalBox", true);
        auto* verticalBoxRect = world->AddComponent<se::ui::components::RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->overridesChildSizes = true;
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        verticalBoxRect->minX = 5;
        verticalBoxRect->minY = 5;
        verticalBoxRect->maxX = 5;
        verticalBoxRect->maxY = 5;
        auto* verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(verticalBoxEntity);
        world->AddChild(contentArea, verticalBoxEntity);

        auto arial = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
        auto textEntity = world->CreateEntity("Text", true);
        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
        text->font = arial;
        text->fontSize = 21;
        text->text = "/assets";
        world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
        se::ui::util::AddVerticalBoxChild(verticalBoxEntity, verticalBox, textEntity);

        m_GridBoxEntity = world->CreateEntity("Grid Box", true);
        auto* gridRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_GridBoxEntity);
        gridRect->overridesChildSizes = true;
        gridRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto* gridBox = world->AddComponent<se::ui::components::GridBoxComponent>(m_GridBoxEntity);
        gridBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(m_GridBoxEntity);
        se::ui::util::AddVerticalBoxChild(verticalBoxEntity, verticalBox, m_GridBoxEntity);

        m_FileTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_file.sass");
        m_FolderTexture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_folder.sass");

        SetActiveFolder("/assets");

        gridBox->dirty = true;
    }

    void AssetBrowserWindow::DestroyUI()
    {

    }

    void AssetBrowserWindow::SetActiveFolder(const String &activeFolder)
    {
        m_ActiveFolder = activeFolder;

        auto world = Application::Get()->GetWorld();
        auto assetManager = se::asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");

        for (const auto &child: world->GetChildren(m_GridBoxEntity))
        {
            world->DestroyEntity(child);
        }

        auto& vfs = io::VFS::Get();
        vfs.ForEachFile(m_ActiveFolder, false, [this, world, assetManager, &arial](const io::VFSFile& file)
        {
            auto fileEntity = CreateFileItem(world, assetManager, file, arial);
            if (fileEntity != ecs::s_InvalidEntity)
            {
                world->AddChild(m_GridBoxEntity, fileEntity);
            }
        });

        auto gridBox = world->GetComponent<se::ui::components::GridBoxComponent>(m_GridBoxEntity);
        gridBox->dirty = true;
    }

    ecs::Id AssetBrowserWindow::CreateFileItem(ecs::World* world,
                                               asset::AssetManager* assetManager,
                                               const io::VFSFile& file,
                                               const std::shared_ptr<asset::Font>& font)
    {
        if (file.fileName.Size() == 0)
        {
            return ecs::s_InvalidEntity;
        }

        ecs::Id fileEntity = world->CreateEntity("File", true);
        auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(fileEntity);
        rect->maxX = 100;
        rect->maxY = 100;

        ecs::Id buttonEntity = world->CreateEntity("Button", true);
        auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->renderingEnabled = false;
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        button->onReleased.Subscribe([this, file]()
        {
            if (file.isDirectory)
            {
                SetActiveFolder(m_ActiveFolder + '/' + file.fileName);
            }
        });
        world->AddChild(fileEntity, buttonEntity);

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

        if (file.isDirectory)
        {
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_FolderTexture);
        }
        else
        {
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &m_FileTexture);
        }

        world->AddChild(fileEntity, imageEntity);

        auto labelEntity = world->CreateEntity("Text", true);
        auto labelText = world->AddComponent<se::ui::components::TextComponent>(labelEntity);
        labelText->font = font;
        labelText->fontSize = 16;
        labelText->text = file.fileName;
        labelText->justification = se::ui::text::Alignment::Center;
        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(labelEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
        textRect->minY = 20;
        world->AddComponent<se::ui::components::WidgetComponent>(labelEntity);
        world->AddChild(fileEntity, labelEntity);

        return fileEntity;
    }
}