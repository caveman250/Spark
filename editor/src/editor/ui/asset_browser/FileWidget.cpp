#include "FileWidget.h"

#include "AssetBrowserUtil.h"
#include "editor/Transactions.h"
#include "editor/components/DragDropComponent.h"
#include "editor/singleton_components/DragDropStateComponent.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/meta/MetaDataManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::editor::ui::asset_browser
{
    asset::AssetReference<asset::Texture> FileWidget::FileTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_file.sass");
    asset::AssetReference<asset::Texture> FileWidget::FolderTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_folder.sass");

    std::shared_ptr<FileWidget> FileWidget::CreateFileWidget(const io::VFSFile& file, const std::shared_ptr<AssetBrowserWindow>& assetBrowser)
    {
        auto fileWidget = std::make_shared<FileWidget>();
        fileWidget->m_File = file;

        const auto app = Application::Get();
        const auto editor = app->GetEditor();
        auto world = app->GetWorld();
        const auto assetManager = asset::AssetManager::Get();

        if (file.fileName.size() == 0)
        {
            return fileWidget;
        }

        fileWidget->m_Id = world->CreateEntity(editor->GetEditorScene(), "File");
        const auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(fileWidget->m_Id);
        rect->minWidth = 100;
        rect->minHeight = 100;
        world->AddComponent<se::ui::components::WidgetComponent>(fileWidget->m_Id);

        auto label= se::ui::util::CreateEditableText(world, "/engine_assets/fonts/CascadiaCode.sass", 14, editor->GetEditorScene());
        fileWidget->m_Label = label.entity;
        label.text->text = file.fileName;
        if (label.text->text.size() > 18)
        {
            label.text->text = label.text->text.substr(0, 16) + "...";
        }
        label.text->alignment = se::ui::text::Alignment::Center;
        se::ui::util::SetEditTextMouseInputEnabled(label.mouseInput, false);
        const auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(label.entity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
        textRect->minY = 20;
        textRect->maxWidth = 140;
        label.text->onComitted.Subscribe([fileWidget, world](std::string newName)
        {
            std::string oldName = fileWidget->m_File.fileName;
            std::weak_ptr weakFileWidget = fileWidget;
            Transactions::Get()->PushAction([weakFileWidget, file = fileWidget->m_File, newName]()
            {
                DoRename(newName, file);
                if (!weakFileWidget.expired())
                {
                    weakFileWidget.lock()->EndRename(newName);
                }
            },
            [weakFileWidget, file = fileWidget->m_File, oldName, newName]()
            {
                const std::string newPath = std::format("{}/{}.sass", file.dir, newName);
                io::VFSFile newFile = file;
                newFile.fileName = newName;
                newFile.fullPath = newPath;
                DoRename(oldName, newFile);
                if (!weakFileWidget.expired())
                {
                    weakFileWidget.lock()->EndRename(oldName);
                }
            });


            auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(fileWidget->m_Label);
            se::ui::util::SetEditTextMouseInputEnabled(mouseInput, false);
        });
        label.text->onCancelled.Subscribe([world, labelEntity = label.entity]()
        {
            auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(labelEntity);
            se::ui::util::SetEditTextMouseInputEnabled(mouseInput, false);
        });
        world->AddChild(fileWidget->m_Id, label.entity);

        const ecs::Id buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Button");
        const auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        const auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        const auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        world->AddChild(fileWidget->m_Id, buttonEntity);

        const ecs::Id imageEntity = world->CreateEntity(editor->GetEditorScene(), "Image");
        const auto imageRect = world->AddComponent<se::ui::components::RectTransformComponent>(imageEntity);
        imageRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        imageRect->maxY = 20;
        imageRect->minAspectRatio = 1.f;
        imageRect->maxAspectRatio = 1.f;
        const auto image = world->AddComponent<se::ui::components::ImageComponent>(imageEntity);
        const std::shared_ptr<render::Material> material = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
        image->materialInstance = std::make_shared<render::MaterialInstance>(material);
        if (file.isDirectory)
        {
            image->materialInstance->SetUniform("Texture", 1, &FolderTexture);
        }
        else
        {
            image->materialInstance->SetUniform("Texture", 1, &FileTexture);
        }
        world->AddChild(fileWidget->m_Id, imageEntity);

        if (!file.isDirectory)
        {
            button->onDragged.Subscribe([fileWidget, world](input::MouseButton)
            {
                const auto editor = Application::Get()->GetEditor();
                const auto dragDropStateComponent = world->GetSingletonComponent<singleton_components::DragDropStateComponent>();
                const auto assetManager = asset::AssetManager::Get();

                const auto entity = world->CreateEntity(editor->GetEditorScene(), "Drag Drop Image");
                world->AddComponent<components::DragDropComponent>(entity);
                auto* rect = world->AddComponent<se::ui::components::RectTransformComponent>(entity);
                rect->layer = -1;
                auto* image = world->AddComponent<se::ui::components::ImageComponent>(entity);
                const std::shared_ptr<render::Material> material = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
                image->materialInstance = std::make_shared<render::MaterialInstance>(material);
                image->materialInstance->SetUniform("Texture", 1, &FileTexture);

                const auto db = asset::binary::Database::Load(fileWidget->GetFile().fullPath.data(), true);
                dragDropStateComponent->dragDropAsset = assetManager->GetAsset(fileWidget->GetFile().fullPath.data(), reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            });
        }

        button->onReleased.Subscribe([fileWidget, labelEntity = label.entity, assetBrowser](const input::MouseButton button)
        {
            const auto& file = fileWidget->GetFile();
            switch (button)
            {
                case input::MouseButton::Left:
                {
                    if (file.isDirectory)
                    {
                        SelectFolder(file.fullPath);
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
                    const auto inputComp = world->GetSingletonComponent<input::InputComponent>();
                    se::ui::util::ContextMenuParams params = {
                        .fontSize = 14,
                        .mousePos = { inputComp->mouseX, inputComp->mouseY },
                        .scene = Application::Get()->GetEditor()->GetEditorScene()
                    };
                    params.AddOption("Rename", [labelEntity, fileName = file.fileName]()
                    {
                        BeginRename(labelEntity, fileName);
                    });
                    std::weak_ptr weakAssetBrowser = assetBrowser;
                    params.AddOption("Duplicate", [file, weakAssetBrowser]()
                    {
                        DuplicateFile(file, weakAssetBrowser);
                    });
                    params.AddOption("Delete", [file, weakAssetBrowser]()
                    {
                        DeleteFile(file, weakAssetBrowser);
                    });
                    se::ui::util::CreateContextMenu(params);
                    break;
                }
                default:
                    break;
            }
        });

        button->onDoubleClick.Subscribe([fileWidget, assetBrowser](const input::MouseButton button)
        {
            const auto& file = fileWidget->GetFile();
            switch (button)
            {
                case input::MouseButton::Left:
                {
                    if (file.isDirectory)
                    {
                        assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder() + '/' + file.fileName, false);
                    }
                    else
                    {
                        OpenFile(file.fullPath);
                    }
                    break;
                }
                default:
                    break;
            }
        });

        return fileWidget;
    }

    void FileWidget::DoRename(const std::string& newName, const io::VFSFile& file)
    {
        auto* app = Application::Get();
        auto* editor = app->GetEditor();
        auto* assetManager = asset::AssetManager::Get();

        const std::string newPath = std::format("{}/{}.sass", file.dir, newName);
        const auto db = asset::binary::Database::Load(file.fullPath.data(), true);
        const auto asset = assetManager->GetAsset(file.fullPath.data(), reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
        editor->RenameAsset(asset, newPath);
        if (editor->GetSelectedAsset() == asset)
        {
            SelectFile(newPath);
        }
    }

    void FileWidget::BeginRename(const ecs::Id& labelEntity, const std::string& fileName)
    {
        auto* world = Application::Get()->GetWorld();
        const auto editText = world->GetComponent<se::ui::components::EditableTextComponent>(labelEntity);
        const auto textRect = world->GetComponent<se::ui::components::RectTransformComponent>(labelEntity);
        const auto keyInputComp = world->GetComponent<se::ui::components::KeyInputComponent>(labelEntity);
        se::ui::util::BeginEditingText(nullptr, labelEntity, *editText, *keyInputComp);
        auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(labelEntity);
        se::ui::util::SetEditTextMouseInputEnabled(mouseInput, true);
        // undo any truncation
        editText->editText = fileName;
        textRect->needsLayout = true;
        se::ui::util::SetCaretPos(*editText, static_cast<int>(editText->editText.size()));
    }

    void FileWidget::EndRename(const std::string& newName)
    {
        auto* world = Application::Get()->GetWorld();
        auto* text = world->GetComponent<se::ui::components::EditableTextComponent>(m_Label);
        text->text = newName;
        if (text->text.size() > 18)
        {
            text->text = text->text.substr(0, 18) + "...";
        }

        const std::string newPath = std::format("{}/{}.sass", m_File.dir, newName);
        m_File.fileName = newName;
        m_File.fullPath = newPath;
    }

    void FileWidget::DuplicateFile(const io::VFSFile& file, const std::weak_ptr<AssetBrowserWindow>& weakAssetBrowser)
    {
        struct DuplicateAssetState
        {
            std::string filePath = {};
        };

        Transactions::Get()->PushAction<DuplicateAssetState>([file, weakAssetBrowser]()
        {
            const auto db = asset::binary::Database::Load(file.fullPath, true);

            const std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                        reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            auto* state = Transactions::Get()->GetRedoState<DuplicateAssetState>();
            state->filePath = Editor::DuplicateAsset(asset);
            if (!weakAssetBrowser.expired())
            {
                auto assetBrowser = weakAssetBrowser.lock();
                assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), false);
            }
            SelectFile(state->filePath);
        },
        [weakAssetBrowser]()
        {
            auto* state = Transactions::Get()->GetUndoState<DuplicateAssetState>();
            const auto db = asset::binary::Database::Load(state->filePath, true);

            const std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(state->filePath,
                                                                                        reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

            auto* editor = Application::Get()->GetEditor();
            editor->DeleteAsset(asset);

            if (!weakAssetBrowser.expired())
            {
                auto assetBrowser = weakAssetBrowser.lock();
                assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), false);
            }
        });
    }

    void FileWidget::DeleteFile(const io::VFSFile& file,
        const std::weak_ptr<AssetBrowserWindow>& weakAssetBrowser)
    {
        const auto db = asset::binary::Database::Load(file.fullPath, true);

        const std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                    reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

        std::shared_ptr<asset::meta::MetaData> meta = nullptr;
        if (asset->UsesMetaData())
        {
            meta = asset::meta::MetaManager::Get()->GetOrCreateMetaDataForAsset(asset.get());
        }

        Transactions::Get()->PushAction([file, weakAssetBrowser]()
        {
            const auto db = asset::binary::Database::Load(file.fullPath, true);

            const std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                        reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            auto* editor = Application::Get()->GetEditor();
            editor->DeleteAsset(asset);

            if (!weakAssetBrowser.expired())
            {
                auto assetBrowser = weakAssetBrowser.lock();
                assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), false);
            }
        },
        [asset, meta, weakAssetBrowser]()
        {
            auto* editor = Application::Get()->GetEditor();
            editor->RestoreAsset(asset, meta);

            if (!weakAssetBrowser.expired())
            {
                auto assetBrowser = weakAssetBrowser.lock();
                assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), false);
            }
        });
    }
}
