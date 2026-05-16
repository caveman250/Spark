#include <editor/ui/properties/util/PropertyUtil.h>
#include "UniformValueEditor.h"

#include "engine/Application.h"
#include "engine/render/UniformStorage.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/asset/shader/ShaderSettings.h"
namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<int>, UniformValueEditor, UniformValue_int)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<float>, UniformValueEditor, UniformValue_float)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<math::Vec2>, UniformValueEditor, UniformValue_Vec2)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<math::Vec3>, UniformValueEditor, UniformValue_Vec3)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<math::Vec4>, UniformValueEditor, UniformValue_Vec4)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<math::Mat3>, UniformValueEditor, UniformValue_Mat3)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<math::Mat4>, UniformValueEditor, UniformValue_Mat4)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<asset::AssetReference<asset::Texture>>, UniformValueEditor, UniformValue_TextureRef)
    DEFINE_PROPERTY_EDITOR(se::render::UniformValue<std::shared_ptr<asset::Texture>>, UniformValueEditor, UniformValue_Texture)

    void UniformValueEditor::SetValue(void* value, const reflect::Type* type)
    {
        m_Value = static_cast<render::UniformValueBase*>(value);
        m_Type = static_cast<const reflect::Class*>(type);
    }

    void UniformValueEditor::ConstructUI(const PropertyEditorParams& params)
    {
        if (!m_Value || m_Value->internal)
        {
            return;
        }

        auto wrappedParams = params;
        wrappedParams.type = m_Type->members[0].type;
        wrappedParams.value = m_Type->members[0].get(m_Value);
        m_WrappedEditor = CreatePropertyEditor(wrappedParams);

        m_TitleMode = params.titleModeOverride != PropertyTitleMode::None ?
            params.titleModeOverride :
            GetDefaultTitleMode();

        if (!m_WrappedEditor)
        {
            PropertyEditor::ConstructUI(params);

            auto app = Application::Get();
            auto world = app->GetWorld();
            auto editor = app->GetEditor();

            auto entity = world->CreateEntity(editor->GetEditorScene(), params.name);
            auto rect = world->AddComponent<RectTransformComponent>(entity);
            rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            rect->minX = 5;
            rect->maxX = 15;
            world->AddComponent<WidgetComponent>(entity);
            world->AddChild(m_Content, entity);

            auto titleMode = m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : GetDefaultTitleMode();
            auto text = properties::util::CreateMissingPropertyEditorText(m_Type->members[0].type,
                                                                          titleMode == PropertyTitleMode::Inline ? 0.35f : 1.f,
                                                                          0);
            world->AddChild(entity, text);
        }
    }

    void UniformValueEditor::Update()
    {
        if (m_WrappedEditor)
        {
            m_WrappedEditor->Update();
        }
    }

    PropertyTitleMode UniformValueEditor::GetDefaultTitleMode() const
    {
        if (m_WrappedEditor)
        {
            return m_WrappedEditor->GetDefaultTitleMode();
        }

        return PropertyTitleMode::Inline;
    }

    ecs::Id UniformValueEditor::GetWidgetId() const
    {
        if (m_WrappedEditor)
        {
            return m_WrappedEditor->GetWidgetId();
        }

        return ecs::InvalidEntity;
    }

    ecs::Id UniformValueEditor::GetTitleId() const
    {
        if (m_WrappedEditor)
        {
            return m_WrappedEditor->GetTitleId();
        }

        return ecs::InvalidEntity;
    }

    void UniformValueEditor::DestroyUI()
    {
        if (m_WrappedEditor)
        {
            m_WrappedEditor->DestroyUI();
        }
    }
}
