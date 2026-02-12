#include "SceneEditor.h"

#include "engine/Application.h"
#include "engine/ecs/SceneSaveData.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(ecs::SceneSaveData, SceneEditor, SceneSaveData);

    void SceneEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<ecs::SceneSaveData*>(value);
    }

    void SceneEditor::ConstructUI(const PropertyEditorParams& params)
    {
        auto paramsCopy = params;
        paramsCopy.name = "Scene";
        PropertyEditor::ConstructUI(paramsCopy);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "SceneEditor Text");
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = std::format("Num Entities: {}", m_Value->m_Entities.size());
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void SceneEditor::Update()
    {
    }
}
