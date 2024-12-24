#pragma once
#include <engine/reflect/Reflect.h>

namespace se::ui::components
{
    struct RectTransformComponent;
}

namespace se::editor::ui::properties
{
    class PropertyEditor;

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes();
    struct PropertyEditorRegister
    {
        PropertyEditorRegister(std::function<void()>&& registerFunc)
        {
            registerFunc();
        }
    };

#define DEFINE_PROPERTY_EDITOR(Type, PropertyEditorType)\
    static PropertyEditorRegister SPARK_CAT(PropertyEditorType, _Register)([](){\
        auto& map = se::editor::ui::properties::GetPropertyEditorTypes();\
        map.insert(std::make_pair(reflect::TypeResolver<Type>::get(), reflect::TypeResolver<PropertyEditorType>::get()));\
    });\

    using namespace se::ui::components;
    class PropertyEditor : public reflect::ObjectBase
    {
    public:
        virtual void ConstructUI() = 0;
        virtual void DestroyUI() = 0;
        virtual void SetValue(void* value) = 0;
        virtual void SetName(const String& name) { m_Name = name; }
        virtual void Update() = 0;

        ecs::Id GetWidgetId() const { return m_WidgetId; }
        RectTransformComponent* GetRectTransform() { return m_RectTransform; }
    protected:
        ecs::Id m_WidgetId;
        RectTransformComponent* m_RectTransform = nullptr;
        String m_Name = {};
    };
}
