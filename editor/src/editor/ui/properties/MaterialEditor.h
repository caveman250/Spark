#pragma once

#include "ClassEditor.h"

namespace se::editor::ui::properties
{
    class MaterialEditor : public ClassEditor
    {
        SPARK_CLASS()

        void Update() override;

        uint8_t m_CachedVertexShaderCount = 0;
        uint8_t m_CachedFragmentShaderCount = 0;
        uint8_t m_CachedUniformsCount = 0;
    };
}
