#pragma once

#include "PropertyEditor.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/texture/Texture.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class CollisionMatrixEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        ecs::Id CreateCheckBox(const math::Vec2& pos, const math::Vec2& size, const string::Symbol& a, const string::Symbol& b);

        void* m_Value = nullptr;
        asset::AssetReference<asset::Texture> m_CheckedTexture = "/engine_assets/textures/checkbox_checked.sass";
        asset::AssetReference<asset::Texture> m_UncheckedTexture = "/engine_assets/textures/checkbox_unchecked.sass";
        std::vector<std::tuple<ecs::Id, string::Symbol, string::Symbol, bool>> m_CheckBoxes = {};
    };
}
