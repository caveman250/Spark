#pragma once

#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class BoolEditor : public PropertyEditor
    {
        DECLARE_SPARK_CLASS(BoolEditor)
    public:
        void SetValue(void* value) override;
        void ConstructUI(const String& name, bool constructTitle) override;
        void Update() override;

    private:
        bool* m_Value = nullptr;
        bool m_LastValue = false;
        ecs::Id m_Tickbox = {};
        std::shared_ptr<asset::Texture> m_CheckedTexture = nullptr;
        std::shared_ptr<asset::Texture> m_UncheckedTexture = nullptr;
    };
}
