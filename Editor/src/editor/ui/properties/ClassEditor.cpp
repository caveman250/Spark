#include <editor/ui/properties/util/PropertyUtil.h>
#include "ClassEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/util/VerticalBoxUtil.h"
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

    void ClassEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors);

        auto world = Application::Get()->GetWorld();

        int numSerialisedMembers = 0;
        auto* verticalBox = world->GetComponent<VerticalBoxComponent>(m_Content);
        for (const auto &member: m_Type->members)
        {
            if (!member.serialized)
            {
                continue;
            }

            numSerialisedMembers++;

            if (auto *propEditor = properties::CreatePropertyEditor(member, m_Value, {0.f, 1.f, 0.f, 0.f}, true))
            {
                se::ui::util::AddVerticalBoxChild(m_Content, verticalBox, propEditor->GetWidgetId());
                m_Editors.push_back(propEditor);
            }
            else
            {
                auto textEntity = properties::util::CreateMissingPropertyEditorText(member.type, 0.f, 0);
                se::ui::util::AddVerticalBoxChild(m_Content, verticalBox, textEntity);
            }
        }

        if (numSerialisedMembers == 0)
        {
            auto textEntity = world->CreateEntity("Text", true);
            auto text = world->AddComponent<se::ui::components::TextComponent>(textEntity);
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 14;
            text->text = "No properties.";
            auto transform = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
            transform->anchors = { 0.f, 1.f, 0.f, 0.f };
            world->AddComponent<se::ui::components::WidgetComponent>(textEntity);
            se::ui::util::AddVerticalBoxChild(m_Content, verticalBox, textEntity);
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
