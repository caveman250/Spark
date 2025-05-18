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

    void MainNodeEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors);

        auto world = Application::Get()->GetWorld();

        auto textEntity = world->CreateEntity("MainNodeEditor Text", true);
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = std::format("Num child nodes: {}", m_Value->m_Children.size());
        text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void MainNodeEditor::Update()
    {
    }
}
