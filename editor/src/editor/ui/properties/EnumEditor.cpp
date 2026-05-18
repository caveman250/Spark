#include "EnumEditor.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/ComboBoxUtil.h"
#include "editor/Transactions.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    void EnumEditor::SetValue(void* value, const reflect::Type* type)
    {
        m_Value = value;
        m_EnumType = static_cast<const reflect::EnumBase*>(type);
    }

    void EnumEditor::ConstructUI(const PropertyEditorParams& params)
    {
       PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/CascadiaCode.sass");

        se::ui::util::ComboBoxParams comboBoxParams =
        {
            .fontSize = 14,
            .options = { },
            .onItemSelected = [this](int item)
            {
                size_t oldVal = m_EnumType->GetIndexForValue(m_Value);
                Transactions::Get()->PushAction([this, item]()
                {
                    m_EnumType->SetValueFromIndex(item, m_Value);
                },
                [oldVal, this]()
                {
                    m_EnumType->SetValueFromIndex(oldVal, m_Value);
                });
            },
            .selectedIndex = static_cast<int>(m_EnumType->GetIndexForValue(m_Value)),
            .scene = editor->GetEditorScene(),
        };

        for (const auto& name : m_EnumType->GetValueNames())
        {
            comboBoxParams.options.push_back(*name);
        }

        auto comboBox = se::ui::util::CreateComboBox(comboBoxParams);
        m_ComboBoxText = comboBox.collapsedText;
        comboBox.rectTransform->anchors = { .left = 0.35f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddChild(m_Content, comboBox.id);
    }

    void EnumEditor::Update()
    {
        auto* world = Application::Get()->GetWorld();
        size_t index = m_EnumType->GetIndexForValue(m_Value);
        if (index != m_LastValueIndex)
        {
            auto* text = world->GetComponent<TextComponent>(m_ComboBoxText);
            text->text = *m_EnumType->GetValueNames()[index];
            m_LastValueIndex = index;
        }
    }
}
