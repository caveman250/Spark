#include "EnumEditor.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/util/ComboBoxUtil.h"

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
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        se::ui::util::ComboBoxParams comboBoxParams =
        {
            .fontSize = 14,
            .options = { },
            .onItemSelected = [this](int item) { m_EnumType->SetValueFromIndex(item, m_Value); },
            .selectedIndex = static_cast<int>(m_EnumType->GetIndexForValue(m_Value)),
            .scene = editor->GetEditorScene(),
        };

        for (const auto& name : m_EnumType->GetValueNames())
        {
            comboBoxParams.options.push_back(*name);
        }

        auto comboBox = se::ui::util::CreateComboBox(comboBoxParams);
        comboBox.rectTransform->anchors = { .left = 0.35f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddChild(m_Content, comboBox.id);
    }

    void EnumEditor::Update()
    {
    }
}
