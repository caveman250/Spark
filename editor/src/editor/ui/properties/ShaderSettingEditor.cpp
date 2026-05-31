#include "ShaderSettingEditor.h"

#include "engine/render/Material.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<int>, ShaderSettingEditor, ShaderSettingDefinition_Int);
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<float>, ShaderSettingEditor, ShaderSettingDefinition_Float);
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<se::math::Vec3>, ShaderSettingEditor, ShaderSettingDefinition_Vec3);
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<se::math::Vec4>, ShaderSettingEditor, ShaderSettingDefinition_Vec4);

    void ShaderSettingEditor::ConstructUI(const PropertyEditorParams& params)
    {
        if (!m_Value || static_cast<ShaderSettingDefinitionBase*>(m_Value)->internal)
        {
            return;
        }

        SingleMemberEditor::ConstructUI(params);
    }
}
