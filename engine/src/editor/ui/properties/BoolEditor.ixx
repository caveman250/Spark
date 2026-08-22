module;

#include "spark.h"

export module BoolEditor;
import AssetReference;
import Spark.Editor.UI.Properties.PropertyEditor;
import Spark.Asset.Texture;


namespace se::editor::ui::properties
{
    class BoolEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
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
