#include "AssetBrowserWindow.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/io/VFS.h"
#include "engine/render/MaterialInstance.h"
#include "engine/string/util/StringUtil.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/GridBoxComponent.h"
#include "engine/ui/components/HorizontalBoxComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ui/components/ScrollViewComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/ScrollBoxUtil.h"
#include "engine/ui/util/TreeViewUtil.h"
#include "engine/ui/util/WindowUtil.h"

namespace se::editor::ui
{
    void AssetBrowserWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();
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
                                                editor->GetEditorScene());
        windowTransform->anchors = {0.f, 0.f, 0.f, 0.f};
        windowTransform->minX = 20;
        windowTransform->maxX = 850;
        windowTransform->minY = 390;
        windowTransform->maxY = 700;

        ecs::Id verticalBoxEntity = world->CreateEntity(editor->GetEditorScene(), "AssetBrowser Container");
        auto* verticalBoxRect = world->AddComponent<se::ui::components::RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        verticalBoxRect->minX = 5;
        verticalBoxRect->minY = 5;
        verticalBoxRect->maxX = 5;
        verticalBoxRect->maxY = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(verticalBoxEntity);
        world->AddChild(contentArea, verticalBoxEntity);

        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(.25f, .25f, .25f));
        }

        auto treeViewBG = world->CreateEntity(editor->GetEditorScene(), "Grid Box BG");
        auto* treeViewRect = world->AddComponent<se::ui::components::RectTransformComponent>(treeViewBG);
        treeViewRect->anchors = { 0.f, 0.f, 0.f, 1.f };
        treeViewRect->maxX = 195;
        auto treeViewBGImage = world->AddComponent<se::ui::components::ImageComponent>(treeViewBG);
        treeViewBGImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(verticalBoxEntity, treeViewBG);

        se::ui::components::RectTransformComponent* transformComp = nullptr;
        se::ui::components::TreeViewComponent* treeViewComp = nullptr;
        auto treeView = ::se::ui::util::CreateTreeView(&treeViewComp, &transformComp, editor->GetEditorScene());
        transformComp->anchors = { 0.f, 1.f, 0.f, 1.f };
        world->AddChild(treeViewBG, treeView);

        for (const auto& mount : io::VFS::Get().GetMounts())
        {
            se::ui::components::TreeNodeComponent* treeNodeComp = nullptr;
            se::ui::components::TextComponent* textComp = nullptr;
            se::ui::util::InsertTreeNode(treeView, treeViewComp, treeView, mount.vfsPath, &treeNodeComp, &textComp, editor->GetEditorScene());
            textComp->text = mount.vfsPath;
            treeNodeComp->onSelected.Subscribe([this, &mount]()
            {
                SetActiveFolder(mount.vfsPath);
            });
        }

        auto pathBarBG = world->CreateEntity(editor->GetEditorScene(), "Path Bar BG");
        auto* pathBarBGRect = world->AddComponent<se::ui::components::RectTransformComponent>(pathBarBG);
        pathBarBGRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        pathBarBGRect->maxY = 30;
        pathBarBGRect->minX = 200;
        auto pathBarBGImage = world->AddComponent<se::ui::components::ImageComponent>(pathBarBG);
        pathBarBGImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(verticalBoxEntity, pathBarBG);

        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        m_PathBarBox = world->CreateEntity(editor->GetEditorScene(), "Path Bar");
        auto pathBarRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_PathBarBox);
        pathBarRect->anchors = { 0.f, 1.f, 0.f, 1.f };
        pathBarRect->minX = 5;
        pathBarRect->overridesChildSizes = true;
        world->AddComponent<se::ui::components::WidgetComponent>(m_PathBarBox);
        auto horBox = world->AddComponent<se::ui::components::HorizontalBoxComponent>(m_PathBarBox);
        horBox->spacing = 5;
        world->AddChild(pathBarBG, m_PathBarBox);

        auto gridBG = world->CreateEntity(editor->GetEditorScene(), "Grid Box BG");
        auto* gridBGRect = world->AddComponent<se::ui::components::RectTransformComponent>(gridBG);
        gridBGRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        gridBGRect->minY = 35;
        gridBGRect->minX = 200;
        auto gridBGImage = world->AddComponent<se::ui::components::ImageComponent>(gridBG);
        gridBGImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(verticalBoxEntity, gridBG);

        ecs::Id scrollViewEntity;
        ecs::Id scrollBarEntity;
        se::ui::components::ScrollBoxComponent *scrollBox = nullptr;
        se::ui::components::ScrollViewComponent *scrollView = nullptr;
        se::ui::components::RectTransformComponent *scrollBoxTransform = nullptr;
        auto scrollBoxEntity = ::se::ui::util::CreateScrollBox(&scrollBox, scrollViewEntity, &scrollView, &scrollBoxTransform, scrollBarEntity, editor->GetEditorScene());
        world->AddChild(gridBG, scrollBoxEntity);

        m_GridBoxEntity = world->CreateEntity(editor->GetEditorScene(), "Grid Box");
        auto* gridRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_GridBoxEntity);
        gridRect->overridesChildSizes = true;
        gridRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto* gridBox = world->AddComponent<se::ui::components::GridBoxComponent>(m_GridBoxEntity);
        gridBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(m_GridBoxEntity);
        world->AddChild(scrollViewEntity, m_GridBoxEntity);

        SetActiveFolder("/assets");

        gridRect->needsLayout = true;
    }

    void AssetBrowserWindow::DestroyUI()
    {

    }

    void AssetBrowserWindow::SetActiveFolder(const std::string &activeFolder)
    {
        m_ActiveFolder = activeFolder;

        auto world = Application::Get()->GetWorld();
        auto assetManager = se::asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        for (const auto &child: world->GetChildren(m_GridBoxEntity))
        {
            world->DestroyEntity(child);
        }

        for (const auto& child: m_PathBarItems)
        {
            world->DestroyEntity(child);
        }
        m_PathBarItems.clear();

        auto& vfs = io::VFS::Get();
        vfs.ForEachFile(m_ActiveFolder, false, [this, world, &arial](const io::VFSFile& file)
        {
            if (file.extension == "json")
            {
                return;
            }

            auto fileEntity = CreateFileItem(world, file, arial);
            if (fileEntity != ecs::s_InvalidEntity)
            {
                world->AddChild(m_GridBoxEntity, fileEntity);
            }
        });

        auto gridBox = world->GetComponent<se::ui::components::RectTransformComponent>(m_GridBoxEntity);
        gridBox->needsLayout = true;

        CreatePathBar(arial);
    }

    void AssetBrowserWindow::CreatePathBar(const std::shared_ptr<asset::Font>& font)
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        std::string cumulativePath = {};
        std::string lhs, rhs;
        std::string path = m_ActiveFolder;
        while (string::util::Split(path, lhs, rhs, '/', false))
        {
            path = rhs;
            if (lhs.size() == 0 && rhs.size() > 0)
            {
                continue;
            }
            cumulativePath += '/';
            cumulativePath += lhs;
            CreatePathItem(world, lhs, cumulativePath, font);

            auto separatorEntity = world->CreateEntity(editor->GetEditorScene(), "Separator");
            auto separatorText = world->AddComponent<se::ui::components::TextComponent>(separatorEntity);
            separatorText->font = font;
            separatorText->fontSize = 18;
            separatorText->text = "/";
            separatorText->alignment = se::ui::text::Alignment::Center;
            auto separatorRect = world->AddComponent<se::ui::components::RectTransformComponent>(separatorEntity);
            separatorRect->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 1.f };
            world->AddComponent<se::ui::components::WidgetComponent>(separatorEntity);
            world->AddChild(m_PathBarBox, separatorEntity);

            m_PathBarItems.push_back(separatorEntity);
        }

        cumulativePath += '/';
        cumulativePath += path;
        CreatePathItem(world, path, cumulativePath, font);

        auto horBox = world->GetComponent<se::ui::components::RectTransformComponent>(m_PathBarBox);
        horBox->needsLayout = true;
    }

    void AssetBrowserWindow::CreatePathItem(ecs::World* world,
                        const std::string& name,
                        const std::string& path,
                        const std::shared_ptr<asset::Font>& font)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();

        ecs::Id buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Button");
        auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        button->onReleased.Subscribe([this, path]()
                                     {
                                         SetActiveFolder(path);
                                     });

        auto labelEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
        auto labelText = world->AddComponent<se::ui::components::TextComponent>(labelEntity);
        labelText->font = font;
        labelText->fontSize = 18;
        labelText->text = name;
        labelText->alignment = se::ui::text::Alignment::Center;
        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(labelEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddComponent<se::ui::components::WidgetComponent>(labelEntity);
        world->AddChild(buttonEntity, labelEntity);

        world->AddChild(m_PathBarBox, buttonEntity);
        m_PathBarItems.push_back(buttonEntity);
    }

    ecs::Id AssetBrowserWindow::CreateFileItem(ecs::World* world,
                                               const io::VFSFile& file,
                                               const std::shared_ptr<asset::Font>& font)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();

        if (file.fileName.size() == 0)
        {
            return ecs::s_InvalidEntity;
        }

        ecs::Id fileEntity = world->CreateEntity(editor->GetEditorScene(), "File");
        auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(fileEntity);
        rect->minWidth = 100;
        rect->minHeight = 100;
        world->AddComponent<se::ui::components::WidgetComponent>(fileEntity);

        ecs::Id buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Button");
        auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        button->onReleased.Subscribe([this, file]()
        {
            if (file.isDirectory)
            {
                SetActiveFolder(m_ActiveFolder + '/' + file.fileName);
            }
            else
            {
                auto app = Application::Get();
                EditorRuntime* runtime = app->GetEditorRuntime();
                auto db = asset::binary::Database::Load(file.fullPath.data(), true);

                std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath.data(),
                                                                                           se::reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
                runtime->SelectAsset(asset);
            }
        });
        world->AddChild(fileEntity, buttonEntity);

        ecs::Id imageEntity = world->CreateEntity(editor->GetEditorScene(), "Image");
        auto imageRect = world->AddComponent<se::ui::components::RectTransformComponent>(imageEntity);
        imageRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        imageRect->maxY = 20;
        imageRect->minAspectRatio = 1.f;
        imageRect->maxAspectRatio = 1.f;
        auto image = world->AddComponent<se::ui::components::ImageComponent>(imageEntity);
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
        image->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(material);

        if (file.isDirectory)
        {
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &m_FolderTexture);
        }
        else
        {
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &m_FileTexture);
        }

        world->AddChild(fileEntity, imageEntity);

        auto labelEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
        auto labelText = world->AddComponent<se::ui::components::TextComponent>(labelEntity);
        labelText->font = font;
        labelText->fontSize = 14;
        labelText->text = file.fileName;
        labelText->alignment = se::ui::text::Alignment::Center;
        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(labelEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
        textRect->minY = 20;
        world->AddComponent<se::ui::components::WidgetComponent>(labelEntity);
        world->AddChild(fileEntity, labelEntity);

        return fileEntity;
    }
}