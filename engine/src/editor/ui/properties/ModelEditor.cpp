#include "ModelEditor.h"

import Application;
#include <engine/ui/components/RectTransformComponent.h>

import Model;

import TextComponent;
#include "engine/ui/util/SplitViewUtil.h"


namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Model, ModelEditor, Model);

    void ModelEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Model*>(value);
    }

    void ModelEditor::ConstructUI(const PropertyEditorParams& params)
    {
        auto metaManager = asset::meta::MetaManager::Get();
        auto meta = metaManager->GetOrCreateMetaDataForAsset(m_Value);

        auto metaType = meta->GetReflectType();
        auto wrappedParams = params;
        wrappedParams.type = metaType;
        wrappedParams.value = meta.get();
        m_WrappedEditor = CreatePropertyEditor(wrappedParams);

        m_TitleMode = params.titleModeOverride != PropertyTitleMode::None ?
            params.titleModeOverride :
            GetDefaultTitleMode();
    }

    void ModelEditor::Update()
    {
        m_WrappedEditor->Update();
    }
}
