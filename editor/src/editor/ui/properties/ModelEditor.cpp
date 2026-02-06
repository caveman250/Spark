#include "ModelEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/asset/mesh/Model.h"
#include "engine/asset/meta/MetaDataManager.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/util/SplitViewUtil.h"
#include "util/PropertyUtil.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Model, ModelEditor, Model);

    void ModelEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Model*>(value);
    }

    void ModelEditor::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        auto metaManager = asset::meta::MetaManager::Get();
        auto meta = metaManager->GetOrCreateMetaDataForAsset(m_Value);

        auto metaType = meta->GetReflectType();
        m_WrappedEditor = CreatePropertyEditor(name,
                                               metaType,
                                               meta.get(),
                                               anchors,
                                               collapsed,
                                               withBackground,
                                               constructTitle);
    }

    void ModelEditor::Update()
    {
    }
}
