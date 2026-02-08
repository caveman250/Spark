#include "AssetBrowserWindow.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/input/InputComponent.h"
#include "engine/io/VFS.h"
#include "engine/render/MaterialInstance.h"
#include "engine/string/util/StringUtil.h"
#include "engine/ui/components/ButtonComponent.h"
#include "../components/DragDropComponent.h"
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
#include "../singleton_components/DragDropStateComponent.h"
#include "engine/ecs/SceneSaveData.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"
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
        auto assetManager = asset::AssetManager::Get();

        se::ui::components::RectTransformComponent* windowTransform;
        se::ui::components::WindowComponent* windowComp;
        se::ui::components::TitleBarComponent* titleBarComp;
        ecs::Id contentArea;
        ecs::Id titleArea;
        m_Window = se::ui::util::CreateWindow(&windowTransform,
                                              &windowComp,
                                              &titleBarComp,
                                              contentArea,
                                              titleArea,
                                              "Asset Browser",
                                              editor->GetEditorScene());
        windowTransform->anchors = { 0.f, 0.f, 0.f, 0.f };
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

        auto treeViewBG = world->CreateEntity(editor->GetEditorScene(), "Grid Box BG");
        auto* treeViewRect = world->AddComponent<se::ui::components::RectTransformComponent>(treeViewBG);
        treeViewRect->anchors = { 0.f, 0.f, 0.f, 1.f };
        treeViewRect->maxX = 195;
        auto treeViewBGImage = world->AddComponent<se::ui::components::ImageComponent>(treeViewBG);
        auto bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_asset_browser_bg.sass");
        treeViewBGImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(verticalBoxEntity, treeViewBG);

        se::ui::components::RectTransformComponent* transformComp = nullptr;
        se::ui::components::TreeViewComponent* treeViewComp = nullptr;
        auto treeView = se::ui::util::CreateTreeView(&treeViewComp, &transformComp, editor->GetEditorScene());
        transformComp->anchors = { 0.f, 1.f, 0.f, 1.f };
        world->AddChild(treeViewBG, treeView);

        for (const auto& mount: io::VFS::Get().GetMounts())
        {
            if (mount.visibleInEditor)
            {
                se::ui::util::TreeNodeParams params = {
                    .treeViewEntity = treeView,
                    .parentNode = treeView,
                    .name = mount.vfsPath,
                    .scene = editor->GetEditorScene(),
                    .treeViewRect = treeViewRect
                };
                se::ui::util::NewTreeNode treeNode = se::ui::util::InsertTreeNode(params);
                treeNode.text->text = mount.vfsPath;
                treeNode.treeNode->onSelected.Subscribe([this, &mount]()
                {
                    SetActiveFolder(mount.vfsPath, false);
                });
            }
        }

        auto pathBarBG = world->CreateEntity(editor->GetEditorScene(), "Path Bar BG");
        auto* pathBarBGRect = world->AddComponent<se::ui::components::RectTransformComponent>(pathBarBG);
        pathBarBGRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        pathBarBGRect->maxY = 30;
        pathBarBGRect->minX = 200;
        auto pathBarBGImage = world->AddComponent<se::ui::components::ImageComponent>(pathBarBG);
        pathBarBGImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(verticalBoxEntity, pathBarBG);

        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        m_PathBarBox = world->CreateEntity(editor->GetEditorScene(), "Path Bar");
        auto pathBarRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_PathBarBox);
        pathBarRect->anchors = { 0.f, 1.f, 0.f, 1.f };
        pathBarRect->minX = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(m_PathBarBox);
        auto horBox = world->AddComponent<se::ui::components::HorizontalBoxComponent>(m_PathBarBox);
        horBox->spacing = 5;
        world->AddChild(pathBarBG, m_PathBarBox);

        m_GridBG = world->CreateEntity(editor->GetEditorScene(), "Grid Box BG");
        auto* gridBGRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_GridBG);
        gridBGRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        gridBGRect->minY = 35;
        gridBGRect->minX = 200;
        auto gridBGImage = world->AddComponent<se::ui::components::ImageComponent>(m_GridBG);
        gridBGImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        auto gridMouseInput = world->AddComponent<se::ui::components::MouseInputComponent>(m_GridBG);
        gridMouseInput->buttonMask = std::to_underlying(se::input::MouseButton::Right);
        world->AddChild(verticalBoxEntity, m_GridBG);

        ecs::Id scrollViewEntity;
        ecs::Id scrollBarEntity;
        se::ui::components::ScrollBoxComponent* scrollBox = nullptr;
        se::ui::components::ScrollViewComponent* scrollView = nullptr;
        se::ui::components::RectTransformComponent* scrollBoxTransform = nullptr;
        auto scrollBoxEntity = se::ui::util::CreateScrollBox(&scrollBox, scrollViewEntity, &scrollView, &scrollBoxTransform, scrollBarEntity, editor->GetEditorScene());
        world->AddChild(m_GridBG, scrollBoxEntity);

        m_GridBoxEntity = world->CreateEntity(editor->GetEditorScene(), "Grid Box");
        auto* gridRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_GridBoxEntity);
        gridRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto* gridBox = world->AddComponent<se::ui::components::GridBoxComponent>(m_GridBoxEntity);
        gridBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(m_GridBoxEntity);
        world->AddChild(scrollViewEntity, m_GridBoxEntity);

        SetActiveFolder("/assets", false);

        gridRect->needsLayout = true;
    }

    void AssetBrowserWindow::DestroyUI()
    {
    }

    void AssetBrowserWindow::Update()
    {
        ToolWindow::Update();

        auto world = Application::Get()->GetWorld();
        auto inputComp = world->GetSingletonComponent<input::InputComponent>();
        auto gridMouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(m_GridBG);
        for (const auto& event: gridMouseInput->mouseEvents)
        {
            if (event.state == input::KeyState::Up)
            {
                se::ui::util::ContextMenuParams params = {
                    .fontSize = 14,
                    .mousePos = { inputComp->mouseX, inputComp->mouseY },
                    .scene = Application::Get()->GetEditorRuntime()->GetEditorScene()
                };
                params.AddOption("Create Scene", [this]()
                {
                    std::string fileName = m_ActiveFolder + "/new_scene.sass";
                    asset::AssetManager::Get()->CreateDataAsset<ecs::SceneSaveData>(fileName);
                    SetActiveFolder(m_ActiveFolder, false);
                    SelectFile(fileName);
                });
                params.AddOption("Create Material", [this]()
                {
                    std::string fileName = m_ActiveFolder + "/new_material.sass";
                    asset::AssetManager::Get()->CreateDataAsset<render::Material>(fileName);
                    SetActiveFolder(m_ActiveFolder, false);
                    SelectFile(fileName);
                });

                se::ui::util::CreateContextMenu(params);
            }
        }
    }

    void AssetBrowserWindow::SetActiveFolder(const std::string& activeFolder, bool setSelection)
    {
        m_ActiveFolder = activeFolder;

        auto world = Application::Get()->GetWorld();

        for (const auto& child: world->GetChildren(m_GridBoxEntity))
        {
            world->DestroyEntity(child);
        }

        for (const auto& child: m_PathBarItems)
        {
            world->DestroyEntity(child);
        }
        m_PathBarItems.clear();

        auto& vfs = io::VFS::Get();
        std::string firstFile = {};
        vfs.ForEachFile(m_ActiveFolder, false, [this, world, &firstFile](const io::VFSFile& file)
        {
            if (file.extension == "json")
            {
                return;
            }

            if (file.fileName.starts_with("."))
            {
                return;
            }

            if (firstFile.empty() && !file.isDirectory)
            {
                firstFile = file.fullPath;
            }

            auto fileEntity = CreateFileItem(world, file, "/engine_assets/fonts/Arial.sass");
            if (fileEntity != ecs::s_InvalidEntity)
            {
                world->AddChild(m_GridBoxEntity, fileEntity);
            }
        }, false, true);

        auto gridBox = world->GetComponent<se::ui::components::RectTransformComponent>(m_GridBoxEntity);
        gridBox->needsLayout = true;

        CreatePathBar("/engine_assets/fonts/Arial.sass");

        if (setSelection && !firstFile.empty())
        {
            SelectFile(firstFile);
        }
    }

    void AssetBrowserWindow::CreatePathBar(const asset::AssetReference<asset::Font>& font)
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        std::string cumulativePath = { };
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
                                            const asset::AssetReference<asset::Font>& font)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();

        ecs::Id pathItemEntity = world->CreateEntity(editor->GetEditorScene(), "Path Item");
        world->AddComponent<se::ui::components::RectTransformComponent>(pathItemEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(pathItemEntity);

        world->AddChild(m_PathBarBox, pathItemEntity);

        ecs::Id buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Button");
        auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        button->onReleased.Subscribe([this, path](input::MouseButton)
        {
            SetActiveFolder(path, false);
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
        world->AddChild(pathItemEntity, labelEntity);
        world->AddChild(pathItemEntity, buttonEntity);

        m_PathBarItems.push_back(pathItemEntity);
    }

    ecs::Id AssetBrowserWindow::CreateFileItem(ecs::World* world,
                                               const io::VFSFile& file,
                                               const asset::AssetReference<asset::Font>& font)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        auto assetManager = asset::AssetManager::Get();

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

        if (!file.isDirectory)
        {
            button->onDragged.Subscribe([this, world, file](input::MouseButton)
            {
                auto editor = Application::Get()->GetEditorRuntime();
                auto dragDropStateComponent = world->GetSingletonComponent<singleton_components::DragDropStateComponent>();
                auto assetManager = asset::AssetManager::Get();

                auto entity = world->CreateEntity(editor->GetEditorScene(), "Drag Drop Image");
                world->AddComponent<components::DragDropComponent>(entity);
                auto* rect = world->AddComponent<se::ui::components::RectTransformComponent>(entity);
                rect->layer = -1;
                auto* image = world->AddComponent<se::ui::components::ImageComponent>(entity);
                std::shared_ptr<render::Material> material = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
                image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &m_FileTexture);

                auto db = asset::binary::Database::Load(file.fullPath.data(), true);
                dragDropStateComponent->dragDropAsset = assetManager->GetAsset(file.fullPath.data(), reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            });
        }

        button->onReleased.Subscribe([this, file](input::MouseButton button)
        {
            switch (button)
            {
                case input::MouseButton::Left:
                {
                    if (file.isDirectory)
                    {
                        SetActiveFolder(m_ActiveFolder + '/' + file.fileName, false);
                    }
                    else
                    {
                        SelectFile(file.fullPath);
                    }
                    break;
                }
                case input::MouseButton::Right:
                {
                    auto world = Application::Get()->GetWorld();
                    auto inputComp = world->GetSingletonComponent<input::InputComponent>();
                    se::ui::util::ContextMenuParams params = {
                        .fontSize = 14,
                        .mousePos = { inputComp->mouseX, inputComp->mouseY },
                        .scene = Application::Get()->GetEditorRuntime()->GetEditorScene()
                    };
                    params.AddOption("Duplicate", [this, file]()
                    {
                        auto db = asset::binary::Database::Load(file.fullPath, true);

                        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                                    reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
                        std::string newPath = EditorRuntime::DuplicateAsset(asset);
                        SetActiveFolder(m_ActiveFolder, false);
                        SelectFile(newPath);
                    });
                    params.AddOption("Delete", [this, file]()
                    {
                        auto db = asset::binary::Database::Load(file.fullPath, true);

                        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                                    reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
                        EditorRuntime::DeleteAsset(asset);
                        SetActiveFolder(m_ActiveFolder, true);
                    });
                    se::ui::util::CreateContextMenu(params);
                    break;
                }
                default:
                    break;
            }
        });
        button->onDoubleClick.Subscribe([this, file](input::MouseButton button)
        {
            switch (button)
            {
                case input::MouseButton::Left:
                {
                    if (!file.isDirectory)
                    {
                        OpenFile(file.fullPath);
                    }
                    break;
                }
                default:
                    break;
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
        std::shared_ptr<render::Material> material = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);

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
        labelText->fontSize = 12;
        labelText->text = file.fileName;
        if (labelText->text.size() > 20)
        {
            labelText->text = labelText->text.substr(0, 20) + "...";
        }
        labelText->alignment = se::ui::text::Alignment::Center;
        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(labelEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
        textRect->minY = 20;
        world->AddComponent<se::ui::components::WidgetComponent>(labelEntity);
        world->AddChild(fileEntity, labelEntity);

        return fileEntity;
    }

    void AssetBrowserWindow::SelectFile(const std::string& file)
    {
        auto app = Application::Get();
        EditorRuntime* runtime = app->GetEditorRuntime();
        auto db = asset::binary::Database::Load(file, true);

        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file,
                                                                                   reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

        runtime->SelectAsset(asset);
    }

    void AssetBrowserWindow::OpenFile(const std::string& file)
    {
        auto app = Application::Get();
        EditorRuntime* runtime = app->GetEditorRuntime();
        auto db = asset::binary::Database::Load(file, true);

        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file,
                                                                                   reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

        if (asset->GetReflectType() == ecs::SceneSaveData::GetReflection())
        {
            runtime->LoadScene(file);
        }
    }
}
