#include "PropertiesWindow.h"

#include <engine/asset/AssetManager.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/TitleBarComponent.h>
#include <engine/ui/components/WidgetComponent.h>
#include <engine/ui/components/WindowComponent.h>
#include <engine/ui/util/RectTransformUtil.h>
#include <engine/ui/util/ScrollBoxUtil.h>
#include <engine/ui/util/WindowUtil.h>
#include <engine/ui/components/VerticalBoxComponent.h>
#include "engine/reflect/Reflect.h"

#include "engine/Application.h"
#include "engine/ui/util/VerticalBoxUtil.h"
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

        se::ui::components::RectTransformComponent *windowTransform;
        se::ui::components::WindowComponent *windowComp;
        se::ui::components::TitleBarComponent *titleBarComp;
        ecs::Id contentArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
                                                &windowComp,
                                                &titleBarComp,
                                                contentArea,
                                                "Properties",
                                                [this]()
                                                { DestroyUI(); },
                                                true);
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
        auto scrollBoxEntity = ::se::ui::util::CreateScrollBox(&scrollBox, scrollViewEntity, &scrollView, &scrollBoxTransform, scrollBarEntity, true);
        world->AddChild(contentArea, scrollBoxEntity);

        m_ScrollBoxContent = world->CreateEntity("Vertical Box");
        auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(m_ScrollBoxContent);
        verticalBox->spacing = 5;
        world->AddComponent<se::ui::components::WidgetComponent>(m_ScrollBoxContent);
        auto verticalBoxTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent);
        verticalBoxTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        verticalBoxTransform->overridesChildSizes = true; // TODO automate this
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

            se::ui::components::VerticalBoxComponent* verticalBox = world->GetComponent<se::ui::components::VerticalBoxComponent>(m_ScrollBoxContent);

            if (selectedEntity)
            {
                AddEntityProperties(selectedEntity, world, ariel, verticalBox);
            }
            else if (selectedSingletonComp)
            {
                AddSingletonComponentProperties(selectedSingletonComp, world, ariel, verticalBox);
            }
            else if (selectedAsset)
            {
                AddAssetProperties(selectedAsset, world, ariel, verticalBox);
            }

            scrollBoxTransform->needsLayout = true;
        }
    }

    void PropertiesWindow::AddEntityProperties(const ecs::Id& entity,
                                               ecs::World* world,
                                               const std::shared_ptr<asset::Font>& font,
                                               se::ui::components::VerticalBoxComponent* verticalBox)
    {
        const auto &selectedEntityRecord = world->m_EntityRecords.at(entity);
        {
            auto textEntity = world->CreateEntity("Text", true);
            auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
            text->font = font;
            text->fontSize = 18;
            text->text = *entity.name;
            world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
            world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
            se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);
        }

        for (auto component: selectedEntityRecord.archetype->type)
        {
            const auto &compRecord = world->m_ComponentRecords[component];
            {
                auto textEntity = world->CreateEntity("Text", true);
                auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                text->font = font;
                text->fontSize = 16;
                text->text = compRecord.type->name;
                world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);
            }

            int numSerialisedMembers = 0;
            for (const auto &member: compRecord.type->members)
            {
                if (!member.serialized)
                {
                    continue;
                }

                numSerialisedMembers++;

                void *instance = world->GetComponent(m_Editor->GetSelectedEntity(), component);

                if (auto *propEditor = properties::CreatePropertyEditor(member, instance, {0.f, 1.f, 0.f, 0.f}, true))
                {
                    se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, propEditor->GetWidgetId());
                    m_PropertyEditors.push_back(propEditor);
                }
                else
                {
                    auto textEntity = properties::util::CreateMissingPropertyEditorText(member.type, 0.f, 0);
                    se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);
                }
            }

            if (numSerialisedMembers == 0)
            {
                auto textEntity = world->CreateEntity("Text", true);
                auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                text->font = font;
                text->fontSize = 14;
                text->text = "No properties.";
                auto transform = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                transform->minX = 10;
                transform->maxX = 15;
                transform->anchors = { 0.f, 1.f, 0.f, 0.f };
                world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);
            }
        }
    }

    void PropertiesWindow::AddSingletonComponentProperties(reflect::ObjectBase* selectedSingletonComp,
                                                           ecs::World* world,
                                                           const std::shared_ptr<asset::Font>& font,
                                                           se::ui::components::VerticalBoxComponent* verticalBox)
    {
        auto reflectClass = static_cast<reflect::Class *>(selectedSingletonComp->GetReflectType());

        auto textEntity = world->CreateEntity("Text", true);
        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
        text->font = font;
        text->fontSize = 16;
        text->text = reflectClass->name;
        world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
        se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);

        for (const auto &member: reflectClass->members)
        {
            if (!member.serialized)
            {
                continue;
            }

            if (auto *propEditor = properties::CreatePropertyEditor(member, selectedSingletonComp, {0.f, 1.f, 0.f, 0.f}, true))
            {
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, propEditor->GetWidgetId());
                m_PropertyEditors.push_back(propEditor);
            }
            else
            {
                auto propTextEntity = properties::util::CreateMissingPropertyEditorText(member.type, 0.f, 0);
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, propTextEntity);
            }
        }
    }

    void PropertiesWindow::AddAssetProperties(const std::shared_ptr<asset::Asset>& asset,
                                              ecs::World* world,
                                              const std::shared_ptr<asset::Font>& font,
                                              se::ui::components::VerticalBoxComponent* verticalBox)
    {
        auto reflectClass = static_cast<reflect::Class *>(asset->GetReflectType());

        auto textEntity = world->CreateEntity("Text", true);
        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
        text->font = font;
        text->fontSize = 16;
        text->text = reflectClass->name;
        world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
        se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, textEntity);

        for (const auto &member: reflectClass->members)
        {
            if (!member.serialized)
            {
                continue;
            }

            if (auto *propEditor = properties::CreatePropertyEditor(member, asset.get(), {0.f, 1.f, 0.f, 0.f}, true))
            {
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, propEditor->GetWidgetId());
                m_PropertyEditors.push_back(propEditor);
            }
            else
            {
                auto propTextEntity = properties::util::CreateMissingPropertyEditorText(member.type, 0.f, 0);
                se::ui::util::AddVerticalBoxChild(m_ScrollBoxContent, verticalBox, propTextEntity);
            }
        }
    }
}
