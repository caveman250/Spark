#pragma once

#include "engine/reflect/Object.h"
#include "engine/ui/Anchors.h"

namespace se::ui
{
    struct Anchors;

    namespace components
    {
        struct EditableTextComponent;
        struct RectTransformComponent;
    }
}

namespace se::editor::ui::properties
{
    SPARK_ENUM()
    enum class PropertyTitleMode
    {
        None,
        Inline,
        NextLine
    };

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes();
    std::unordered_map<std::string, reflect::Type*>& GetContainerPropertyEditorTypes();

    class PropertyEditor;
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
        map.insert(std::make_pair(reflect::TypeResolver<Type>::Get(), reflect::TypeResolver<PropertyEditorType>::Get()));\
    });                                                 \

#define DEFINE_CONTAINER_PROPERTY_EDITOR(Type, PropertyEditorType)\
    static PropertyEditorRegister SPARK_CAT(SPARK_CAT(PropertyEditorType, _Register), __COUNTER__)([](){\
        auto& map = se::editor::ui::properties::GetContainerPropertyEditorTypes();\
        map.insert(std::make_pair(Type, reflect::TypeResolver<PropertyEditorType>::Get()));\
    });

#if SPARK_PLATFORM_MAC || SPARK_PLATFORM_LINUX
#define SSCANF(buffer, str, var, ...) sscanf(buffer, str, var)
#else
#define SSCANF(buffer, str, var, ...) sscanf_s(buffer, str, var, __VA_ARGS__)
#endif

    struct PropertyEditorParams
    {
        std::string name = {};
        reflect::Type* type = nullptr;
        void* value = nullptr;
        se::ui::Anchors anchors = {};
        bool collapsed = false;
        bool withBackground = false;
        bool constructTitle = false;
        PropertyTitleMode titleModeOverride = PropertyTitleMode::None;
        bool editableTitle = false;
        std::vector<std::pair<std::string, std::function<void()>>> contextOptions = {};
    };

    using namespace se::ui::components;
    class PropertyEditor : public reflect::ObjectBase, public std::enable_shared_from_this<PropertyEditor>
    {
    public:
        virtual void ConstructUI(const PropertyEditorParams& params);
        virtual void DestroyUI();
        virtual void* GetValue() const = 0;
        virtual void SetValue(void* value, const reflect::Type* type) = 0;
        virtual void SetName(const std::string& name) { m_Name = name; }
        void UpdateName(const std::string& name);
        virtual void Update() = 0;
        PropertyTitleMode GetTitleMode() const;
        virtual PropertyTitleMode GetDefaultTitleMode() const { return PropertyTitleMode::Inline; }

        virtual void BeginRename(const std::string_view editText,
                                 const std::function<void(const std::string&, EditableTextComponent*)>& onComitted,
                                 const std::function<void(EditableTextComponent*)>& onCancelled);

        virtual ecs::Id GetWidgetId() const { return m_WidgetId; }
        virtual ecs::Id GetTitleId() const { return m_Title; }
        virtual ecs::Id GetContentId() const { return m_Content; }
        RectTransformComponent* GetRectTransform() { return m_RectTransform; }
    protected:
        ecs::Id m_WidgetId;
        ecs::Id m_Content;
        ecs::Id m_Title;
        RectTransformComponent* m_RectTransform = nullptr;
        std::string m_Name = {};
        uint64_t m_RenameComittedHandle = {};
        uint64_t m_RenameCancelledHandle = {};
        PropertyTitleMode m_TitleMode = PropertyTitleMode::Inline;
    };

    std::shared_ptr<PropertyEditor> CreatePropertyEditor(const PropertyEditorParams& params);
}
