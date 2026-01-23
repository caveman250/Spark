#include "PropertiesWindow.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/ScrollBoxUtil.h"
#include "engine/ui/util/WindowUtil.h"
#include "properties/util/PropertyUtil.h"

namespace se::editor::ui
{
    void PropertiesWindow::Update()
    {
        if (!m_Valid)
        {
            return;
        }

        ToolWindow::Update();

        for (auto *editor: m_PropertyEditors)
        {
            editor->Update();
        }
    }

    void PropertiesWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        se::ui::components::RectTransformComponent *windowTransform;
        se::ui::components::WindowComponent *windowComp;
        se::ui::components::TitleBarComponent *titleBarComp;
        ecs::Id contentArea;
        ecs::Id titleArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
                                                &windowComp,
                                                &titleBarComp,
                                                contentArea,
                                                titleArea,
                                                "Properties",
                                                editor->GetEditorScene());
        windowTransform->anchors = {0.f, 0.f, 0.f, 0.f};
        windowTransform->minX = 860;
        windowTransform->maxX = 1260;
        windowTransform->minY = 20;
        windowTransform->maxY = 700;

        ecs::Id scrollViewEntity;
        ecs::Id scrollBarEntity;
        se::ui::components::ScrollBoxComponent *scrollBox = nullptr;
        se::ui::components::ScrollViewComponent *scrollView = nullptr;
        se::ui::components::RectTransformComponent *scrollBoxTransform = nullptr;
        auto scrollBoxEntity = ::se::ui::util::CreateScrollBox(&scrollBox, scrollViewEntity, &scrollView, &scrollBoxTransform, scrollBarEntity, editor->GetEditorScene());
        world->AddChild(contentArea, scrollBoxEntity);

        m_ScrollBoxContent = world->CreateEntity("Vertical Box");
        auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(m_ScrollBoxContent);
        verticalBox->spacing = 5;
        verticalBox->paddingLeft = 2;
        verticalBox->paddingRight = 15;
        world->AddComponent<se::ui::components::WidgetComponent>(m_ScrollBoxContent);
        auto verticalBoxTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent);
        verticalBoxTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        world->AddChild(scrollViewEntity, m_ScrollBoxContent);

        m_Valid = true;
    }

    void PropertiesWindow::DestroyUI()
    {
        m_Valid = false;
    }

    void PropertiesWindow::RebuildProperties()
    {
        if (m_Valid)
        {
            auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            auto world = Application::Get()->GetWorld();
            for (const auto &child: world->GetChildren(m_ScrollBoxContent))
            {
                world->DestroyEntity(child);
            }
            for (const auto &propertyEditor: m_PropertyEditors)
            {
                delete propertyEditor;
            }
            m_PropertyEditors.clear();

            auto scrollBoxTransform = world->GetComponent<se::ui::components::RectTransformComponent>(
                    m_ScrollBoxContent);

            ecs::Id selectedEntity = m_Editor->GetSelectedEntity();
            reflect::ObjectBase* selectedSingletonComp = m_Editor->GetSelectedSingletonComponent();
            const auto& selectedAsset = m_Editor->GetSelectedAsset();

            if (selectedEntity)
            {
                AddEntityProperties(selectedEntity, world, ariel);
            }
            else if (selectedSingletonComp)
            {
                AddSingletonComponentProperties(selectedSingletonComp);
            }
            else if (selectedAsset)
            {
                AddAssetProperties(selectedAsset, world, ariel);
            }

            scrollBoxTransform->needsLayout = true;
        }
    }

    void PropertiesWindow::AddEntityProperties(const ecs::Id& entity,
                                               ecs::World* world,
                                               const std::shared_ptr<asset::Font>& font)
    {
        auto editor = Application::Get()->GetEditorRuntime();

        const auto &selectedEntityRecord = world->m_EntityRecords.at(entity);
        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
            text->font = font;
            text->fontSize = 18;
            text->text = *entity.name;
            world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
            world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
            world->AddChild(m_ScrollBoxContent, textEntity);
        }

        for (auto component : selectedEntityRecord.archetype->typeVector)
        {
            const auto &compRecord = world->m_ComponentRecords[component];
            auto compInstance = world->GetComponent(entity, component);
            if (auto *propEditor = properties::CreatePropertyEditor(compRecord.type->GetTypeName(compInstance), compRecord.type, compInstance, {0.f, 1.f, 0.f, 0.f}, false, true, true))
            {
                world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
                m_PropertyEditors.push_back(propEditor);
            }
            else
            {
                auto propTextEntity = properties::util::CreateMissingPropertyEditorText(compRecord.type, 0.f, 0);
                world->AddChild(m_ScrollBoxContent, propTextEntity);
            }
        }
    }

    void PropertiesWindow::AddSingletonComponentProperties(reflect::ObjectBase* selectedSingletonComp)
    {
        auto world = Application::Get()->GetWorld();
        auto reflectClass = static_cast<reflect::Class *>(selectedSingletonComp->GetReflectType());

        if (auto *propEditor = properties::CreatePropertyEditor(reflectClass->GetTypeName(selectedSingletonComp), reflectClass, selectedSingletonComp, {0.f, 1.f, 0.f, 0.f}, false, true, true))
        {
            world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
            m_PropertyEditors.push_back(propEditor);
        }
        else
        {
            auto propTextEntity = properties::util::CreateMissingPropertyEditorText(reflectClass, 0.f, 0);
            world->AddChild(m_ScrollBoxContent, propTextEntity);
        }
    }

    void PropertiesWindow::AddAssetProperties(const std::shared_ptr<asset::Asset>& asset,
                                              ecs::World* world,
                                              const std::shared_ptr<asset::Font>& font)
    {
        auto editor = Application::Get()->GetEditorRuntime();

        auto reflectClass = static_cast<reflect::Class *>(asset->GetReflectType());

        auto filePathEntity = world->CreateEntity(editor->GetEditorScene(), "File Path");
        auto filePathText = world->AddComponent<se::ui::components::TextComponent>(filePathEntity);
        filePathText->font = font;
        filePathText->fontSize = 12;
        filePathText->text = asset->m_Path;
        world->AddComponent<se::ui::components::RectTransformComponent>(filePathEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(filePathEntity);
        world->AddChild(m_ScrollBoxContent, filePathEntity);

        if (auto *propEditor = properties::CreatePropertyEditor(reflectClass->GetTypeName(asset.get()), reflectClass, asset.get(), {0.f, 1.f, 0.f, 0.f}, false, true, true))
        {
            world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
            m_PropertyEditors.push_back(propEditor);
        }
        else
        {
            auto propTextEntity = properties::util::CreateMissingPropertyEditorText(reflectClass, 0.f, 0);
            world->AddChild(m_ScrollBoxContent, propTextEntity);
        }
    }
}
