#include "PropertiesWindow.h"

#include <engine/asset/AssetManager.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/TitleBarComponent.h>
#include <engine/ui/components/TreeViewComponent.h>
#include <engine/ui/components/WidgetComponent.h>
#include <engine/ui/components/WindowComponent.h>
#include <engine/ui/util/RectTransformUtil.h>
#include <engine/ui/util/ScrollBoxUtil.h>
#include <engine/ui/util/TreeViewUtil.h>
#include <engine/ui/util/WindowUtil.h>
#include "engine/bits/FlagUtil.h"
#include "engine/ecs/World.h"
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
            auto scrollBoxTransform = world->GetComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent);
            const auto& selectedEntityRecord = world->m_EntityRecords[m_Editor->GetSelectedEntity()];
            float yOffset = 0;
            for (auto component : selectedEntityRecord.archetype->type)
            {
                const auto& compRecord = world->m_ComponentRecords[component];
                {
                    auto textEntity = world->CreateEntity("Text");
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

                for (const auto& member : compRecord.type->members)
                {
                    void* instance = world->GetComponent(m_Editor->GetSelectedEntity(), component);

                    if (auto* propEditorType = properties::GetPropertyEditorTypes()[member.type])
                    {
                        properties::PropertyEditor* propertyEditor = static_cast<properties::PropertyEditor*>(propEditorType->heap_constructor());
                        propertyEditor->SetValue(member.get(instance));
                        propertyEditor->SetName(member.name);
                        propertyEditor->ConstructUI();
                        world->AddChild(m_ScrollBoxContent, propertyEditor->GetWidgetId());
                        auto propEditorRect = propertyEditor->GetRectTransform();
                        se::ui::Rect rect = se::ui::util::CalculateScreenSpaceRect(*propEditorRect, *scrollBoxTransform);
                        propEditorRect->minY = yOffset;
                        propEditorRect->maxY = rect.size.y + yOffset;
                        yOffset += rect.size.y;
                        m_PropertyEditors.push_back(propertyEditor);
                    }
                    else
                    {
                        auto textEntity = world->CreateEntity("Text");
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

            world->GetComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent)->needsLayout = true;
        }
    }
}
