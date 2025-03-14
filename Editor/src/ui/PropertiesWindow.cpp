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
#include "engine/reflect/Reflect.h"

#include "engine/Application.h"

namespace se::editor::ui
{
    void PropertiesWindow::Update()
    {
        if (!m_Valid)
        {
            return;
        }

        ToolWindow::Update();

        for (auto* editor : m_PropertyEditors)
        {
            editor->Update();
        }
    }

    void PropertiesWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        se::ui::components::RectTransformComponent* windowTransform;
        se::ui::components::WindowComponent* windowComp;
        se::ui::components::TitleBarComponent* titleBarComp;
        ecs::Id contentArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
            &windowComp,
            &titleBarComp,
            contentArea,
            "Properties",
            [this](){ DestroyUI(); },
            true);
        windowTransform->anchors = { 0.f, 0.f, 0.f, 0.f };
        windowTransform->minX = 800;
        windowTransform->maxX = 1200;
        windowTransform->minY = 200;
        windowTransform->maxY = 720;

        se::ui::components::ScrollBoxComponent* scrollBox = nullptr;
        se::ui::components::RectTransformComponent* scrollBoxTransform = nullptr;
        m_ScrollBoxContent = ::se::ui::util::CreateScrollBox(&scrollBox, &scrollBoxTransform, true);
        world->AddChild(contentArea, m_ScrollBoxContent);

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
            auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");
            auto world = Application::Get()->GetWorld();
            for (const auto& child : world->GetChildren(m_ScrollBoxContent))
            {
                world->DestroyEntity(child);
            }
            for (const auto& propertyEditor : m_PropertyEditors)
            {
                delete propertyEditor;
            }
            m_PropertyEditors.clear();

            auto scrollBoxTransform = world->GetComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent);

            ecs::Id selectedEntity = m_Editor->GetSelectedEntity();
            reflect::ObjectBase* selectedSingletonComp = m_Editor->GetSelectedSingletonComponent();

            if (selectedEntity)
            {
                const auto& selectedEntityRecord = world->m_EntityRecords.at(m_Editor->GetSelectedEntity());
                int yOffset = 0;
                {
                    auto textEntity = world->CreateEntity("Text", true);
                    auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                    text->font = ariel;
                    text->fontSize = 24;
                    text->text = *m_Editor->GetSelectedEntity().name;
                    auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                    textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                    textRect->minX = 2;
                    textRect->maxX = 2;
                    textRect->minY = 0;
                    textRect->maxY = 24;
                    world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                    world->AddChild(m_ScrollBoxContent, textEntity);
                    yOffset+= 24 + 6;
                }

                for (auto component : selectedEntityRecord.archetype->type)
                {
                    const auto& compRecord = world->m_ComponentRecords[component];
                    {
                        auto textEntity = world->CreateEntity("Text", true);
                        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                        text->font = ariel;
                        text->fontSize = 21;
                        text->text = compRecord.type->name;
                        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                        textRect->minX = 2;
                        textRect->maxX = 2;
                        textRect->minY = yOffset;
                        textRect->maxY = 21 + 2 + yOffset;
                        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                        world->AddChild(m_ScrollBoxContent, textEntity);
                        yOffset+= 21 + 2;
                    }

                    if (compRecord.type->members.empty())
                    {
                        auto textEntity = world->CreateEntity("Text", true);
                        auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                        text->font = ariel;
                        text->fontSize = 18;
                        text->text = "No properties.";
                        auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                        textRect->minX = 2;
                        textRect->maxX = 2;
                        textRect->minY = yOffset;
                        textRect->maxY = 18 + 2 + yOffset;
                        world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                        world->AddChild(m_ScrollBoxContent, textEntity);
                        yOffset+= 18 + 2;
                    }
                    else
                    {
                        for (const auto& member : compRecord.type->members)
                        {
                            if (!member.serialized)
                            {
                                continue;
                            }

                            void* instance = world->GetComponent(m_Editor->GetSelectedEntity(), component);

                            if (auto* propEditor = properties::CreatePropertyEditor(member, instance))
                            {
                                world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
                                auto propEditorRect = propEditor->GetRectTransform();
                                se::ui::Rect rect = se::ui::util::CalculateScreenSpaceRect(*propEditorRect, *scrollBoxTransform);
                                propEditorRect->minY = yOffset;
                                propEditorRect->maxY = rect.size.y + yOffset;
                                yOffset += rect.size.y;
                                m_PropertyEditors.push_back(propEditor);
                            }
                            else
                            {
                                auto textEntity = world->CreateEntity("Text", true);
                                auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                                text->font = ariel;
                                text->fontSize = 18;
                                text->text = std::format("Missing Property Editor of type {}.", member.type->GetTypeName(nullptr));
                                auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                                textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                                textRect->minX = 12;
                                textRect->maxX = 0;
                                textRect->minY = yOffset;
                                textRect->maxY = 18 + yOffset;
                                world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                                world->AddChild(m_ScrollBoxContent, textEntity);
                                yOffset += 18;
                            }
                        }
                    }
                }
            }
            else if (selectedSingletonComp)
            {
                auto reflectClass = static_cast<reflect::Class*>(selectedSingletonComp->GetReflectType());
                int yOffset = 0;

                auto textEntity = world->CreateEntity("Text", true);
                auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
                text->font = ariel;
                text->fontSize = 21;
                text->text = reflectClass->name;
                auto textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
                textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                textRect->minX = 2;
                textRect->maxX = 2;
                textRect->minY = yOffset;
                textRect->maxY = 21 + 2 + yOffset;
                world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
                world->AddChild(m_ScrollBoxContent, textEntity);
                yOffset+= 21 + 2;

                for (const auto& member : reflectClass->members)
                    {
                        if (!member.serialized)
                        {
                            continue;
                        }

                        if (auto* propEditor = properties::CreatePropertyEditor(member, selectedSingletonComp))
                        {
                            world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
                            auto propEditorRect = propEditor->GetRectTransform();
                            se::ui::Rect rect = se::ui::util::CalculateScreenSpaceRect(*propEditorRect, *scrollBoxTransform);
                            propEditorRect->minY = yOffset;
                            propEditorRect->maxY = rect.size.y + yOffset;
                            yOffset += rect.size.y;
                            m_PropertyEditors.push_back(propEditor);
                        }
                        else
                        {
                            auto propTextEntity = world->CreateEntity("Text", true);
                            auto propText = world->AddComponent<se::ui::components::TextComponent>(propTextEntity);
                            propText->font = ariel;
                            propText->fontSize = 18;
                            propText->text = std::format("Missing Property Editor of type {}.", member.type->GetTypeName(nullptr));
                            auto propTextRect = world->AddComponent<se::ui::components::RectTransformComponent>(propTextEntity);
                            propTextRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                            propTextRect->minX = 12;
                            propTextRect->maxX = 0;
                            propTextRect->minY = yOffset;
                            propTextRect->maxY = 18 + yOffset;
                            world->AddComponent<se::ui::components::WidgetComponent>(propTextEntity);
                            world->AddChild(m_ScrollBoxContent, propTextEntity);
                            yOffset += 18;
                        }
                    }
            }

            scrollBoxTransform->needsLayout = true;
        }
    }
}
