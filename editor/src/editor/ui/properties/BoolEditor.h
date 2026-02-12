#pragma once

#include "PropertyEditor.h"
#include "engine/asset/AssetReference.h"
#include "engine/math/math.h"

namespace se::asset
{
    class Texture;
}

namespace se::editor::ui::properties
{
    class BoolEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        bool* m_Value = nullptr;
        bool m_LastValue = false;
        ecs::Id m_Tickbox = {};
        asset::AssetReference<asset::Texture> m_CheckedTexture = {};
        asset::AssetReference<asset::Texture> m_UncheckedTexture = {};
    };
}
