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
    std::unordered_map<std::string, reflect::Type*>& GetContainerPropertyEditorTypes()
    {
        static std::unordered_map<std::string, reflect::Type*> s_Instance = { };
        return s_Instance;
    }

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes()
    {
        static std::unordered_map<reflect::Type*, reflect::Type*> s_Instance = { };
        return s_Instance;
    }

    void PropertyEditor::ConstructUI(const PropertyEditorParams& params)
    {
        constexpr int titleFontSize = 14;
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        m_Name = params.name;

        ecs::Id titleContainer = { };
        if (GetTitleMode() == PropertyTitleMode::NextLine)
        {
            ecs::Id contentsEntity;
            CollapsingHeaderComponent* collapsingHeader = nullptr;
            m_WidgetId = se::ui::util::CreateCollapsingHeader(world, m_Name, titleContainer, contentsEntity, &collapsingHeader, params.collapsed, params.withBackground, editor->GetEditorScene());
            m_RectTransform = world->GetComponent<RectTransformComponent>(m_WidgetId);
            m_RectTransform->anchors = params.anchors;

            m_Content = world->CreateEntity(editor->GetEditorScene(), std::format("Property Content ({})", params.name));
            auto contentRect = world->AddComponent<RectTransformComponent>(m_Content);
            contentRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentRect->minX = 5;
            world->AddComponent<WidgetComponent>(m_Content);
            auto verticalBox = world->AddComponent<VerticalBoxComponent>(m_Content);
            verticalBox->spacing = 10;
            world->AddChild(contentsEntity, m_Content);
        }
        else
        {
            m_WidgetId = world->CreateEntity(editor->GetEditorScene(), params.name);
            m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
            m_RectTransform->anchors = params.anchors;
            world->AddComponent<WidgetComponent>(m_WidgetId);
            titleContainer = m_WidgetId;
            m_Content = m_WidgetId;
        }

        if (params.constructTitle)
        {
            auto titleEntity = world->CreateEntity(editor->GetEditorScene(), std::format("Property Title ({})", params.name));
            auto titleText = world->AddComponent<TextComponent>(titleEntity);
            titleText->font = "/engine_assets/fonts/Arial.sass";
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

    PropertyEditor* CreatePropertyEditor(const PropertyEditorParams& params)
    {
        reflect::Type* editor_type = nullptr;

        if (params.type->IsEnum())
        {
            editor_type = EnumEditor::GetReflection();
        }
        else if (params.type->IsContainer())
        {
            auto* container = static_cast<reflect::Type_Container*>(params.type);
            editor_type = GetContainerPropertyEditorTypes()[container->GetContainerTypeName()];
        }
        else
        {
            editor_type = GetPropertyEditorTypes()[params.type];
        }

        if (!editor_type)
        {
            if (params.type->IsClass())
            {
                editor_type = reflect::TypeResolver<ClassEditor>::get();
            }
            else
            {
                return nullptr;
            }
        }

        auto editor = static_cast<PropertyEditor*>(editor_type->heap_constructor());
        editor->SetValue(params.value, params.type);
        editor->SetName(params.name);
        editor->ConstructUI(params);
        return editor;
    }
}
