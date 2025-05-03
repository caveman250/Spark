#include <editor/ui/properties/util/PropertyUtil.h>
#include "SharedPtrEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/util/VerticalBoxUtil.h"
#include "engine/asset/AssetManager.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::shared_ptr<>", SharedPtrEditor)

    DEFINE_SPARK_CLASS_BEGIN(SharedPtrEditor)
    DEFINE_SPARK_CLASS_END(SharedPtrEditor)

    void SharedPtrEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_Type = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void SharedPtrEditor::ConstructUI(const String& name, bool constructTitle)
    {
        PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();

        auto containedType = m_Type->GetContainedValueType();
        auto propertyEditor = CreatePropertyEditor(name,
                                                   containedType,
                                                   m_Type->GetContainedValue(m_Value));

        if (!propertyEditor)
        {
            auto text = properties::util::CreateMissingPropertyEditorText(containedType, constructTitle ? 0.5f : 0.f);
            world->AddChild(m_WidgetId, text);
        }
        else
        {
            propertyEditor->GetRectTransform()->anchors.left = constructTitle ? 0.5f : 0.f;
            world->AddChild(m_WidgetId, propertyEditor->GetWidgetId());
            m_RectTransform->maxY = propertyEditor->GetRectTransform()->maxY;
        }
    }

    void SharedPtrEditor::Update()
    {
    }
}
