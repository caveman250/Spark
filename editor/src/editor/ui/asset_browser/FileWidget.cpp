#include "FileWidget.h"

#include "AssetBrowserUtil.h"
#include "editor/components/DragDropComponent.h"
#include "editor/singleton_components/DragDropStateComponent.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/GridBoxComponent.h"
#include "engine/ui/components/HorizontalBoxComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ui/components/ScrollViewComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::editor::ui::asset_browser
{
    asset::AssetReference<asset::Texture> FileWidget::FileTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_file.sass");
    asset::AssetReference<asset::Texture> FileWidget::FolderTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_folder.sass");

    FileWidget* FileWidget::CreateFileWidget(const io::VFSFile& file, AssetBrowserWindow* assetBrowser)
    {
        FileWidget* fileWidget = new FileWidget();
        fileWidget->m_File = file;

        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        auto world = app->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        if (file.fileName.size() == 0)
        {
            return fileWidget;
        }

        fileWidget->m_Id = world->CreateEntity(editor->GetEditorScene(), "File");
        auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(fileWidget->m_Id);
        rect->minWidth = 100;
        rect->minHeight = 100;
        world->AddComponent<se::ui::components::WidgetComponent>(fileWidget->m_Id);

        auto label= se::ui::util::CreateEditableText(world, "/engine_assets/fonts/Arial.sass", 12);
        label.text->text = file.fileName;
        if (label.text->text.size() > 18)
        {
            label.text->text = label.text->text.substr(0, 18) + "...";
        }
        label.text->alignment = se::ui::text::Alignment::Center;
        se::ui::util::SetEditTextMouseInputEnabled(label.mouseInput, false);
        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(label.entity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 1.f, .bottom = 1.f };
        textRect->minY = 20;
        textRect->maxWidth = 140;
        label.text->onComitted.Subscribe([fileWidget, world, editor, assetManager, labelEntity = label.entity](std::string newName)
        {
            auto file = fileWidget->m_File;
            std::string newPath = std::format("{}/{}.sass", file.dir, newName);
            auto db = asset::binary::Database::Load(file.fullPath.data(), true);
            auto asset = assetManager->GetAsset(file.fullPath.data(), reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            editor->RenameAsset(asset, newPath);
            if (editor->GetSelectedAsset() == asset)
            {
                SelectFile(newPath);
            }

            file.fileName = newName;
            file.fullPath = newPath;
            fileWidget->SetFile(file);

            auto* text = world->GetComponent<se::ui::components::EditableTextComponent>(labelEntity);
            if (text->text.size() > 18)
            {
                text->text = text->text.substr(0, 18) + "...";
            }

            auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(labelEntity);
            se::ui::util::SetEditTextMouseInputEnabled(mouseInput, false);
        });
        world->AddChild(fileWidget->m_Id, label.entity);

        ecs::Id buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Button");
        auto buttonRect = world->AddComponent<se::ui::components::RectTransformComponent>(buttonEntity);
        buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto buttonWidget = world->AddComponent<se::ui::components::WidgetComponent>(buttonEntity);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(buttonEntity);
        world->AddChild(fileWidget->m_Id, buttonEntity);

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
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &FolderTexture);
        }
        else
        {
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &FileTexture);
        }
        world->AddChild(fileWidget->m_Id, imageEntity);

        if (!file.isDirectory)
        {
            button->onDragged.Subscribe([fileWidget, world](input::MouseButton)
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
                image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &FileTexture);

                auto db = asset::binary::Database::Load(fileWidget->GetFile().fullPath.data(), true);
                dragDropStateComponent->dragDropAsset = assetManager->GetAsset(fileWidget->GetFile().fullPath.data(), reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
            });
        }

        button->onReleased.Subscribe([fileWidget, labelEntity = label.entity, assetBrowser](input::MouseButton button)
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
                    auto inputComp = world->GetSingletonComponent<input::InputComponent>();
                    se::ui::util::ContextMenuParams params = {
                        .fontSize = 14,
                        .mousePos = { inputComp->mouseX, inputComp->mouseY },
                        .scene = Application::Get()->GetEditorRuntime()->GetEditorScene()
                    };
                    params.AddOption("Rename", [world, labelEntity, fileName = file.fileName]()
                    {
                        auto editText = world->GetComponent<se::ui::components::EditableTextComponent>(labelEntity);
                        auto textRect = world->GetComponent<se::ui::components::RectTransformComponent>(labelEntity);
                        auto keyInputComp = world->GetComponent<se::ui::components::KeyInputComponent>(labelEntity);
                        se::ui::util::BeginEditingText(nullptr, labelEntity, *editText, *keyInputComp);
                        auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(labelEntity);
                        se::ui::util::SetEditTextMouseInputEnabled(mouseInput, true);
                        // undo any truncation
                        editText->editText = fileName;
                        textRect->needsLayout = true;
                        se::ui::util::SetCaretPos(*editText, static_cast<int>(editText->editText.size()));
                    });
                    params.AddOption("Duplicate", [file, assetBrowser]()
                    {
                        auto db = asset::binary::Database::Load(file.fullPath, true);

                        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                                    reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
                        std::string newPath = EditorRuntime::DuplicateAsset(asset);
                        assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), false);
                        SelectFile(newPath);
                    });
                    params.AddOption("Delete", [file, assetBrowser]()
                    {
                        auto db = asset::binary::Database::Load(file.fullPath, true);

                        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file.fullPath,
                                                                                                    reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));
                        EditorRuntime::DeleteAsset(asset);
                        assetBrowser->SetActiveFolder(assetBrowser->GetActiveFolder(), true);
                    });
                    se::ui::util::CreateContextMenu(params);
                    break;
                }
                default:
                    break;
            }
        });

        button->onDoubleClick.Subscribe([fileWidget, assetBrowser](input::MouseButton button)
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
}
