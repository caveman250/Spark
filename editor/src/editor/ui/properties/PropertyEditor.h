#pragma once
#include "engine/reflect/Object.h"

namespace se::ui
{
    struct Anchors;

    namespace components
    {
        struct RectTransformComponent;
    }
}

namespace se::editor::ui::properties
{
    SPARK_ENUM()
    enum class PropertyTitleMode
    {
        Inline,
        NextLine
    };

    class PropertyEditor;

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes();
    std::unordered_map<String, reflect::Type*>& GetContainerPropertyEditorTypes();
    struct PropertyEditorRegister
    {
        PropertyEditorRegister(std::function<void()>&& registerFunc)
        {
            registerFunc();
        }
    };

#define DEFINE_PROPERTY_EDITOR(Type, PropertyEditorType, UnqualifiedName)\
    static PropertyEditorRegister SPARK_CAT(PropertyEditor_, SPARK_CAT(UnqualifiedName, _Register))([](){\
        auto& map = se::editor::ui::properties::GetPropertyEditorTypes();\
        map.insert(std::make_pair(reflect::TypeResolver<Type>::get(), reflect::TypeResolver<PropertyEditorType>::get()));\
    });                                                 \

#define DEFINE_CONTAINER_PROPERTY_EDITOR(Type, PropertyEditorType)\
    static PropertyEditorRegister SPARK_CAT(SPARK_CAT(PropertyEditorType, _Register), __COUNTER__)([](){\
        auto& map = se::editor::ui::properties::GetContainerPropertyEditorTypes();\
        map.insert(std::make_pair(Type, reflect::TypeResolver<PropertyEditorType>::get()));\
    });

#if SPARK_PLATFORM_MAC || SPARK_PLATFORM_LINUX
#define SSCANF(buffer, str, var, ...) sscanf(buffer, str, var)
#else
#define SSCANF(buffer, str, var, ...) sscanf_s(buffer, str, var, __VA_ARGS__)
#endif

    using namespace se::ui::components;
    class PropertyEditor : public reflect::ObjectBase
    {
    public:
        virtual void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground);
        virtual void DestroyUI();
        virtual void SetValue(void* value, const reflect::Type* type) = 0;
        virtual void SetName(const String& name) { m_Name = name; }
        virtual void Update() = 0;
        virtual PropertyTitleMode GetTitleMode() const { return PropertyTitleMode::Inline; }

        virtual ecs::Id GetWidgetId() const { return m_WidgetId; }
        RectTransformComponent* GetRectTransform() { return m_RectTransform; }
    protected:
        ecs::Id m_WidgetId;
        ecs::Id m_Content;
        RectTransformComponent* m_RectTransform = nullptr;
        String m_Name = {};
    };

    PropertyEditor* CreatePropertyEditor(const String& name, reflect::Type* type, void* value, const se::ui::Anchors& anchors, bool collapsed, bool withBackground, bool constructTitle);
    PropertyEditor* CreatePropertyEditor(const reflect::Class::Member& member, const void* classInstance, const se::ui::Anchors& anchors, bool collapsed, bool withBackground, bool constructTitle);
}
