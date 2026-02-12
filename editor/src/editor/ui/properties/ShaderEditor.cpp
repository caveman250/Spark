#include "ShaderEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/asset/shader/ast/TypeUtil.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Shader, ShaderEditor, Shader);

    void ShaderEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Shader*>(value);
    }

    void ShaderEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "ShaderEditor Text");
        auto* textRect = world->AddComponent<se::ui::components::RectTransformComponent>(textEntity);
        textRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->text = "Input Ports:\n";
        const auto& inputPorts = m_Value->GetInputPorts();
        if (inputPorts.empty())
        {
            text->text += "    None.\n";
        }
        else
        {
            for (const auto& port: inputPorts | std::views::values)
            {
                text->text += std::format("    {}\n", port->GetPortName());
            }
        }

        text->text += "\nOutput Ports:\n";
        const auto& outputPorts = m_Value->GetOutputPorts();
        if (outputPorts.empty())
        {
            text->text += "    None.\n";
        }
        else
        {
            for (const auto& port: outputPorts | std::views::values)
            {
                text->text += std::format("    {}\n", port->GetPortName());
            }
        }

        text->text += "\nSettings:\n";
        const auto& settings = m_Value->GetSettingVariables();
        if (settings.empty())
        {
            text->text += "    None.\n";
        }
        else
        {
            for (const auto& setting: settings)
            {
                text->text += std::format("    {} {}\n", asset::shader::ast::TypeUtil::TypeToGlsl(setting.second.type), setting.first);
            }
        }

        text->text += "\nUniforms:\n";
        const auto& uniforms = m_Value->GetUniformVariables();
        if (uniforms.empty())
        {
            text->text += "    None.\n";
        }
        else
        {
            for (const auto& uniform: uniforms)
            {
                text->text += std::format("    {} {}\n", asset::shader::ast::TypeUtil::TypeToGlsl(uniform.second.type), uniform.first);
            }
        }

        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        world->AddChild(m_Content, textEntity);
    }

    void ShaderEditor::Update()
    {
    }
}
