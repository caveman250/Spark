#include "PropertyEditor.h"

#include "ClassEditor.h"
#include "EnumEditor.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/text/WrapMode.h"
#include "engine/ui/util/CollapsingHeaderUtil.h"

namespace se::ui::components
{
    struct WidgetComponent;
}

namespace se::editor::ui::properties
{
    std::unordered_map<reflect::Type*, reflect::Type*> PropertyEditorRegister::s_PropertyEditorTypes = {};
    std::unordered_map<String, reflect::Type*> PropertyEditorRegister::s_ContainerPropertyEditorTypes = {};

    void PropertyEditor::ConstructUI(const String& name,
                                     bool constructTitle,
                                     const se::ui::Anchors& anchors,
                                     bool collapsed,
                                     bool withBackground)
    {
        constexpr int titleFontSize = 14;
        auto world = Application::Get()->GetWorld();
        auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        m_Name = name;

        ecs::Id titleContainer = { };
        if (GetTitleMode() == PropertyTitleMode::NextLine)
        {
            ecs::Id contentsEntity;
            CollapsingHeaderComponent* collapsingHeader = nullptr;
            m_WidgetId = se::ui::util::CreateCollapsingHeader(world, titleContainer, contentsEntity, &collapsingHeader, collapsed, withBackground, true);
            m_RectTransform = world->GetComponent<RectTransformComponent>(m_WidgetId);
            m_RectTransform->anchors = anchors;

            m_Content = world->CreateEntity("Vector Editor Vertical Box", true);
            auto contentRect = world->AddComponent<RectTransformComponent>(m_Content);
            contentRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentRect->minX = 10;
            contentRect->overridesChildSizes = true;
            world->AddComponent<WidgetComponent>(m_Content);
            auto verticalBox = world->AddComponent<VerticalBoxComponent>(m_Content);
            verticalBox->spacing = 10;
            world->AddChild(contentsEntity, m_Content);
        }
        else
        {
            m_WidgetId = world->CreateEntity(name, true);
            m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
            m_RectTransform->anchors = anchors;
            world->AddComponent<WidgetComponent>(m_WidgetId);
            titleContainer = m_WidgetId;
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
            titleRect->anchors = {
                .left = 0.f,
                .right = GetTitleMode() == PropertyTitleMode::Inline ? 0.3f : 1.f,
                .top = 0.f,
                .bottom = 1.f
            };
            world->AddChild(titleContainer, titleEntity);
        }
    }

    void PropertyEditor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    PropertyEditor* CreatePropertyEditor(const String& name,
                                         reflect::Type* type,
                                         void* value,
                                         const se::ui::Anchors& anchors,
                                         bool collapsed,
                                         bool withBackground,
                                         bool constructTitle)
    {
        reflect::Type* editor_type = nullptr;

        if (type->IsEnum())
        {
            editor_type = EnumEditor::GetReflection();
        }
        else if (type->IsContainer())
        {
            auto* container = static_cast<reflect::Type_Container*>(type);
            editor_type = PropertyEditorRegister::s_ContainerPropertyEditorTypes[container->GetContainerTypeName()];
        }
        else
        {
            editor_type = PropertyEditorRegister::s_PropertyEditorTypes[type];
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
        editor->ConstructUI(name, constructTitle, anchors, collapsed, withBackground);
        return editor;
    }

    PropertyEditor* CreatePropertyEditor(const reflect::Class::Member& member,
                                         const void* classInstance,
                                         const se::ui::Anchors& anchors,
                                         bool collapsed,
                                         bool withBackground,
                                         bool constructTitle)
    {
        return CreatePropertyEditor(member.name, member.type, member.get(classInstance), anchors, collapsed, withBackground, constructTitle);
    }
}
