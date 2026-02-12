#include "MainNodeEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/asset/shader/ast/MainNode.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::shader::ast::MainNode, MainNodeEditor, MainNode);

    void MainNodeEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::shader::ast::MainNode*>(value);
    }

    void MainNodeEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "MainNodeEditor Text");
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = std::format("Num child nodes: {}", m_Value->m_Children.size());
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void MainNodeEditor::Update()
    {
    }
}
