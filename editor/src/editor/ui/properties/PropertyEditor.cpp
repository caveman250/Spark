#include "PropertyEditor.h"
#include "ClassEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/text/WrapMode.h"

namespace se::ui::components
{
    struct WidgetComponent;
}

namespace se::editor::ui::properties
{
    std::unordered_map<String, reflect::Type*>& GetContainerPropertyEditorTypes()
    {
        static std::unordered_map<String, reflect::Type*> s_Instance = {};
        return s_Instance;
    }

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes()
    {
        static  std::unordered_map<reflect::Type*, reflect::Type*> s_Instance = {};
        return s_Instance;
    }

    void PropertyEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        constexpr int titleFontSize = 14;
        auto world = Application::Get()->GetWorld();
        auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        m_Name = name;

        m_WidgetId = world->CreateEntity(name, true);
        m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
        m_RectTransform->anchors = anchors;
        world->AddComponent<WidgetComponent>(m_WidgetId);

        if (GetTitleMode() == PropertyTitleMode::NextLine)
        {
            m_Content = world->CreateEntity("Vector Editor Vertical Box", true);
            auto contentRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_Content);
            contentRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentRect->minY = constructTitle ? titleFontSize + 2 : 0;
            contentRect->minX = 10;
            contentRect->overridesChildSizes = true;
            world->AddComponent<WidgetComponent>(m_Content);
            auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(m_Content);
            verticalBox->spacing = 5;
            verticalBox->paddingBottom = 10;
            verticalBox->dirty = true;
            world->AddChild(m_WidgetId, m_Content);
        }
        else
        {
            m_Content = m_WidgetId;
        }

        if (constructTitle)
        {
            auto titleEntity = world->CreateEntity("Property Title", true);
            auto titleText = world->AddComponent<TextComponent>(titleEntity);
            titleText->font = ariel;
            titleText->fontSize = titleFontSize;
            titleText->text = m_Name;
            titleText->wrap = se::ui::text::WrapMode::Char;
            auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
            titleRect->anchors = { .left = 0.f,
                                   .right = GetTitleMode() == PropertyTitleMode::Inline ? 0.3f : 1.f,
                                   .top = 0.f,
                                   .bottom = 0.f };
            world->AddChild(m_WidgetId, titleEntity);
        }
    }

    void PropertyEditor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    PropertyEditor* CreatePropertyEditor(const String& name, reflect::Type* type, void* value, const se::ui::Anchors& anchors, bool constructTitle)
    {
        reflect::Type* editor_type = nullptr;

        if (type->IsContainer())
        {
            auto* container = static_cast<reflect::Type_Container*>(type);
            auto& map = GetContainerPropertyEditorTypes();
            editor_type = map[container->GetContainerTypeName()];
        }
        else
        {
            auto& map = GetPropertyEditorTypes();
            editor_type = map[type];
        }

        if (!editor_type)
        {
            if (type->IsClass())
            {
                editor_type = reflect::TypeResolver<ClassEditor>::get();
            }
            else
            {
                return nullptr;
            }
        }

        auto editor = static_cast<PropertyEditor*>(editor_type->heap_constructor());
        editor->SetValue(value, type);
        editor->SetName(name);
        editor->ConstructUI(name, constructTitle, anchors);
        return editor;
    }

    PropertyEditor* CreatePropertyEditor(const reflect::Class::Member& member, const void* classInstance, const se::ui::Anchors& anchors, bool constructTitle)
    {
        return CreatePropertyEditor(member.name, member.type, member.get(classInstance), anchors, constructTitle);
    }
}
