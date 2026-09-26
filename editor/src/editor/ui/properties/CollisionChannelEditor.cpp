#include "CollisionChannelEditor.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/ComboBoxUtil.h"
#include "editor/Transactions.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(geo::CollisionChannel, CollisionChannelEditor, CollisionChannel);

    void CollisionChannelEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<geo::CollisionChannel*>(value);
    }

    void CollisionChannelEditor::ConstructUI(const PropertyEditorParams& params)
    {
       PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/CascadiaCode.sass");

        const auto& collisionSettings = geo::CollisionSettings::Get();
        se::ui::util::ComboBoxParams comboBoxParams =
        {
            .fontSize = 14,
            .options = { },
            .onItemSelected = [this, collisionSettings](int item)
            {
                geo::CollisionChannel oldVal = *m_Value;
                Transactions::Get()->PushAction([this, item, collisionSettings]()
                {
                    *m_Value = collisionSettings.GetChannel(item);
                },
                [oldVal, this]()
                {
                    *m_Value = oldVal;
                });
            },
            .selectedIndex = static_cast<int>(collisionSettings.GetChannelIndex(m_Value->channel)),
            .scene = editor->GetEditorScene(),
        };

        for (const auto& name : collisionSettings.channels)
        {
            comboBoxParams.options.push_back(name.ToString());
        }

        auto comboBox = se::ui::util::CreateComboBox(comboBoxParams);
        m_ComboBoxText = comboBox.collapsedText;
        comboBox.rectTransform->anchors = { .left = 0.35f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddChild(m_Content, comboBox.id);
    }

    void CollisionChannelEditor::Update()
    {
        auto* world = Application::Get()->GetWorld();
        if (*m_Value != m_LastValue)
        {
            auto* text = world->GetComponent<TextComponent>(m_ComboBoxText);
            text->text = m_Value->channel.ToStringView();
            m_LastValue = *m_Value;
        }
    }
}
