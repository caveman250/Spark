#pragma once

#include "SingleMemberEditor.h"

namespace se::editor::ui::properties
{
    class ShaderSettingEditor : public SingleMemberEditor
    {
        SPARK_CLASS()

        void ConstructUI(const PropertyEditorParams& params) override;
    };
}
