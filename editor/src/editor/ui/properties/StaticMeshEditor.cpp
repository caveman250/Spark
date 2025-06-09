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

    void StaticMeshEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto world = Application::Get()->GetWorld();

        auto textEntity = world->CreateEntity("StaticMeshEditor Text", true);
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = std::format("Num Vertices: {}\nNum Indices: {}\nNum Normals: {}\nNum UVs: {}",
            m_Value->vertices.size(),
            m_Value->indices.size(),
            m_Value->uvs.size(),
            m_Value->normals.size());
        text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void StaticMeshEditor::Update()
    {
    }
}
