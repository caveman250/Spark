#include "StaticMeshEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::StaticMesh, StaticMeshEditor, StaticMesh);

    void StaticMeshEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::StaticMesh*>(value);
    }

    void StaticMeshEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "StaticMeshEditor Text");
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = std::format("Num Vertices: {}\nNum Indices: {}\nNum Normals: {}\nNum UVs: {}",
            m_Value->vertices.size(),
            m_Value->indices.size(),
            m_Value->uvs.size(),
            m_Value->normals.size());
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void StaticMeshEditor::Update()
    {
    }
}
