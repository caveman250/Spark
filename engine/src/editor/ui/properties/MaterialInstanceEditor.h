#pragma once

#include "ClassEditor.h"

namespace se::editor::ui::properties
{
    class MaterialInstanceEditor : public ClassEditor
    {
        SPARK_CLASS()

        void Update() override;

        uint8_t m_CachedUniformsCount = 0;
    };
}
