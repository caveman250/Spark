#include "PropertyEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>
#include <engine/ui/util/MeshUtil.h>
#include <engine/ui/util/RectTransformUtil.h>

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

    void PropertyEditor::ConstructUI(const String& name, bool constructTitle)
    {
        auto world = Application::Get()->GetWorld();
        auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        m_WidgetId = world->CreateEntity(name, true);
        m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
        m_RectTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        m_RectTransform->minX = 5;
        m_RectTransform->maxX = 15;
        world->AddComponent<WidgetComponent>(m_WidgetId);

        if (constructTitle)
        {
            auto titleEntity = world->CreateEntity("Title", true);
            auto titleText = world->AddComponent<TextComponent>(titleEntity);
            titleText->font = ariel;
            titleText->fontSize = 18;
            titleText->text = m_Name;
            auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
            titleRect->anchors = { .left = 0.f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
            titleRect->minX = 2;
            titleRect->minY = 1;
            titleRect->maxY = 22;
            world->AddChild(m_WidgetId, titleEntity);
        }
    }

    void PropertyEditor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    PropertyEditor* CreatePropertyEditor(const String& name, reflect::Type* type, void* value)
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
            return nullptr;
        }

        auto editor = static_cast<PropertyEditor*>(editor_type->heap_constructor());
        editor->SetValue(value, type);
        editor->SetName(name);
        editor->ConstructUI(name, true);
        return editor;
    }

    PropertyEditor* CreatePropertyEditor(const reflect::Class::Member& member, const void* classInstance)
    {
        return CreatePropertyEditor(member.name, member.type, member.get(classInstance));
    }
}
