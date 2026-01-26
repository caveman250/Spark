#include <editor/ui/properties/util/PropertyUtil.h>
#include "ClassEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/asset/AssetManager.h"

namespace se::editor::ui::properties
{
    void ClassEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsClass()))
        {
            m_Type = static_cast<const reflect::Class*>(type);
            m_Value = value;
        }
    }

    void ClassEditor::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        int numSerialisedMembers = 0;
        for (const auto &member: m_Type->members)
        {
            if (!member.serialized)
            {
                continue;
            }

            numSerialisedMembers++;

            if (auto *propEditor = CreatePropertyEditor(member, m_Value, {0.f, 1.f, 0.f, 0.f}, true, false, true))
            {
                world->AddChild(m_Content, propEditor->GetWidgetId());
                m_Editors.push_back(propEditor);
            }
            else
            {
                auto textEntity = util::CreateMissingPropertyEditorText(member.type, 0.f, 0);
                world->AddChild(m_Content, textEntity);
            }
        }

        if (numSerialisedMembers == 0)
        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Text");
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            text->text = "No properties.";
            auto transform = world->AddComponent<RectTransformComponent>(textEntity);
            transform->anchors = { 0.f, 1.f, 0.f, 0.f };
            world->AddComponent<WidgetComponent>(textEntity);
            world->AddChild(m_Content, textEntity);
        }
    }

    void ClassEditor::Update()
    {
        for (auto* editor : m_Editors)
        {
            editor->Update();
        }
    }
}
