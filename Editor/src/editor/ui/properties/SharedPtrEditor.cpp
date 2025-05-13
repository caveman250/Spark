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

    void SharedPtrEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        auto containedType = m_Type->GetContainedValueType();
        m_WrappedEditor = CreatePropertyEditor(name,
                                               containedType,
                                               m_Type->GetContainedValue(m_Value),
                                               { 0.f, 1.f, 0.f, 0.f },
                                               false);

        PropertyEditor::ConstructUI(name, constructTitle, anchors);

        auto world = Application::Get()->GetWorld();

        auto entity = world->CreateEntity(name, true);
        auto rect = world->AddComponent<RectTransformComponent>(entity);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 5;
        rect->maxX = 15;
        world->AddComponent<WidgetComponent>(entity);
        world->AddChild(m_WidgetId, entity);

        auto titleMode = m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : PropertyTitleMode::Inline;

        if (!m_WrappedEditor)
        {
            auto text = properties::util::CreateMissingPropertyEditorText(containedType,
                                                                          titleMode == PropertyTitleMode::Inline ? 0.3f : 1.f,
                                                                          0);
            world->AddChild(entity, text);
        }
        else
        {
            world->AddChild(entity, m_WrappedEditor->GetWidgetId());
            auto* propertyRect = m_WrappedEditor->GetRectTransform();
            m_RectTransform->maxY = propertyRect->maxY;
        }
    }

    void SharedPtrEditor::Update()
    {
    }
}
