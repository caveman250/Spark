#include "EditableTextSystem.h"

#include "engine/render/Renderer.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/util/TextUtil.h"
#include "platform/IWindow.h"

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(EditableTextSystem)

    void HandleKey(components::EditableTextComponent& text,
                    components::KeyInputComponent& keyInput,
                   input::Key::Type key)
    {
        switch (key)
        {
            case input::Key::Space:
                text.editText.Insert(text.caretPosition, ' ');
                text.caretPosition++;
                break;
            case input::Key::Apostrophe:
                text.editText.Insert(text.caretPosition, '\'');
                text.caretPosition++;
                break;
            case input::Key::Comma:
                text.editText.Insert(text.caretPosition, ',');
                text.caretPosition++;
                break;
            case input::Key::Minus:
                text.editText.Insert(text.caretPosition, '-');
                text.caretPosition++;
                break;
            case input::Key::Period:
                text.editText.Insert(text.caretPosition, '.');
                text.caretPosition++;
                break;
            case input::Key::ForwardSlash:
                text.editText.Insert(text.caretPosition, '/');
                text.caretPosition++;
                break;
            case input::Key::Key0:
                text.editText.Insert(text.caretPosition, '0');
                text.caretPosition++;
                break;
            case input::Key::Key1:
                text.editText.Insert(text.caretPosition, '1');
                text.caretPosition++;
                break;
            case input::Key::Key2:
                text.editText.Insert(text.caretPosition, '2');
                text.caretPosition++;
                break;
            case input::Key::Key3:
                text.editText.Insert(text.caretPosition, '3');
                text.caretPosition++;
                break;
            case input::Key::Key4:
                text.editText.Insert(text.caretPosition, '4');
                text.caretPosition++;
                break;
            case input::Key::Key5:
                text.editText.Insert(text.caretPosition, '5');
                text.caretPosition++;
                break;
            case input::Key::Key6:
                text.editText.Insert(text.caretPosition, '6');
                text.caretPosition++;
                break;
            case input::Key::Key7:
                text.editText.Insert(text.caretPosition, '7');
                text.caretPosition++;
                break;
            case input::Key::Key8:
                text.editText.Insert(text.caretPosition, '8');
                text.caretPosition++;
                break;
            case input::Key::Key9:
                text.editText.Insert(text.caretPosition, '9');
                text.caretPosition++;
                break;
            case input::Key::Semicolon:
                text.editText.Insert(text.caretPosition, ';');
                text.caretPosition++;
                break;
            case input::Key::Equal:
                text.editText.Insert(text.caretPosition, '=');
                text.caretPosition++;
                break;
            case input::Key::A:
                text.editText.Insert(text.caretPosition, 'a');
                text.caretPosition++;
                break;
            case input::Key::B:
                text.editText.Insert(text.caretPosition, 'b');
                text.caretPosition++;
                break;
            case input::Key::C:
                text.editText.Insert(text.caretPosition, 'c');
                text.caretPosition++;
                break;
            case input::Key::D:
                text.editText.Insert(text.caretPosition, 'd');
                text.caretPosition++;
                break;
            case input::Key::E:
                text.editText.Insert(text.caretPosition, 'e');
                text.caretPosition++;
                break;
            case input::Key::F:
                text.editText.Insert(text.caretPosition, 'f');
                text.caretPosition++;
                break;
            case input::Key::G:
                text.editText.Insert(text.caretPosition, 'g');
                text.caretPosition++;
                break;
            case input::Key::H:
                text.editText.Insert(text.caretPosition, 'h');
                text.caretPosition++;
                break;
            case input::Key::I:
                text.editText.Insert(text.caretPosition, 'i');
                text.caretPosition++;
                break;
            case input::Key::J:
                text.editText.Insert(text.caretPosition, 'j');
                text.caretPosition++;
                break;
            case input::Key::K:
                text.editText.Insert(text.caretPosition, 'k');
                text.caretPosition++;
                break;
            case input::Key::L:
                text.editText.Insert(text.caretPosition, 'l');
                text.caretPosition++;
                break;
            case input::Key::M:
                text.editText.Insert(text.caretPosition, 'm');
                text.caretPosition++;
                break;
            case input::Key::N:
                text.editText.Insert(text.caretPosition, 'n');
                text.caretPosition++;
                break;
            case input::Key::O:
                text.editText.Insert(text.caretPosition, 'o');
                text.caretPosition++;
                break;
            case input::Key::P:
                text.editText.Insert(text.caretPosition, 'p');
                text.caretPosition++;
                break;
            case input::Key::Q:
                text.editText.Insert(text.caretPosition, 'q');
                text.caretPosition++;
                break;
            case input::Key::R:
                text.editText.Insert(text.caretPosition, 'r');
                text.caretPosition++;
                break;
            case input::Key::S:
                text.editText.Insert(text.caretPosition, 's');
                text.caretPosition++;
                break;
            case input::Key::T:
                text.editText.Insert(text.caretPosition, 't');
                text.caretPosition++;
                break;
            case input::Key::U:
                text.editText.Insert(text.caretPosition, 'u');
                text.caretPosition++;
                break;
            case input::Key::V:
                text.editText.Insert(text.caretPosition, 'v');
                text.caretPosition++;
                break;
            case input::Key::W:
                text.editText.Insert(text.caretPosition, 'w');
                text.caretPosition++;
                break;
            case input::Key::X:
                text.editText.Insert(text.caretPosition, 'x');
                text.caretPosition++;
                break;
            case input::Key::Y:
                text.editText.Insert(text.caretPosition, 'y');
                text.caretPosition++;
                break;
            case input::Key::Z:
                text.editText.Insert(text.caretPosition, 'z');
                text.caretPosition++;
                break;
            case input::Key::LeftBracket:
                text.editText.Insert(text.caretPosition, '[');
                text.caretPosition++;
                break;
            case input::Key::Backslash:
                text.editText.Insert(text.caretPosition, '\\');
                text.caretPosition++;
                break;
            case input::Key::RightBracket:
                text.editText.Insert(text.caretPosition, ']');
                text.caretPosition++;
                break;
            case input::Key::GraveAccent:
                text.editText.Insert(text.caretPosition, '`');
                text.caretPosition++;
                break;
            case input::Key::World2:
                break;
            case input::Key::Escape:
                break;
            case input::Key::Enter:
                text.onComitted.Broadcast(text.editText);
                text.text = text.editText;
                text.inEditMode = false;
                keyInput.keyMask = static_cast<input::Key::Type>(0);
                break;
            case input::Key::Tab:
                text.editText.Insert(text.caretPosition, "    ");
                break;
            case input::Key::Backspace:
                if (text.caretPosition > 0)
                {
                    text.editText.Erase(text.caretPosition - 1);
                    text.caretPosition = std::clamp<int>(--text.caretPosition, 0, text.editText.Size());
                }
                break;
            case input::Key::Delete:
                if (text.caretPosition < text.editText.Size())
                {
                    text.editText.Erase(text.caretPosition);
                }
                break;
            case input::Key::Right:
                text.caretPosition = std::clamp<size_t>(text.caretPosition + 1, 0, text.editText.Size());
                break;
            case input::Key::Left:
                text.caretPosition = std::clamp<size_t>(text.caretPosition - 1, 0, text.editText.Size());
                break;
            case input::Key::Down:
            case input::Key::Up:
            case input::Key::PageUp:
            case input::Key::PageDown:
            case input::Key::Home:
            case input::Key::End:
            case input::Key::CapsLock:
            case input::Key::ScrollLock:
            case input::Key::NumLock:
            case input::Key::PrintScreen:
            case input::Key::Pause:
            case input::Key::Insert:
            case input::Key::F1:
            case input::Key::F2:
            case input::Key::F3:
            case input::Key::F4:
            case input::Key::F5:
            case input::Key::F6:
            case input::Key::F7:
            case input::Key::F8:
            case input::Key::F9:
            case input::Key::F10:
            case input::Key::F11:
            case input::Key::F12:
            case input::Key::F13:
            case input::Key::F14:
            case input::Key::F15:
            case input::Key::F16:
            case input::Key::F17:
            case input::Key::F18:
            case input::Key::F19:
            case input::Key::F20:
            case input::Key::F21:
            case input::Key::F22:
            case input::Key::F23:
            case input::Key::F24:
            case input::Key::Num0:
            case input::Key::Num1:
            case input::Key::Num2:
            case input::Key::Num3:
            case input::Key::Num4:
            case input::Key::Num5:
            case input::Key::Num6:
            case input::Key::Num7:
            case input::Key::Num8:
            case input::Key::Num9:
            case input::Key::NumDecimal:
            case input::Key::NumDivide:
            case input::Key::NumMultiply:
            case input::Key::NumSubtract:
            case input::Key::NumAdd:
            case input::Key::NumEnter:
            case input::Key::NumEqual:
            case input::Key::LeftShift:
            case input::Key::LeftControl:
            case input::Key::LeftAlt:
            case input::Key::LeftSuper:
            case input::Key::RightShift:
            case input::Key::RightControl:
            case input::Key::RightAlt:
            case input::Key::RightSuper:
            case input::Key::Menu:
            case input::Key::Unknown:
                break;
        }
    }

    void EditableTextSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        auto app = Application::Get();
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto window = app->GetPrimaryWindow();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        const auto& entities = updateData.GetEntities();
        const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
        const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* textComps = updateData.GetComponentArray<components::EditableTextComponent>();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();
        const auto* mouseInputComps = updateData.GetComponentArray<const components::MouseInputComponent>();
        auto* keyInputComps = updateData.GetComponentArray<components::KeyInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const ecs::Id& entity = entities[i];
            const components::WidgetComponent& widget = widgetComps[i];
            const components::RectTransformComponent& transform = transformComps[i];
            components::EditableTextComponent& text = textComps[i];
            components::KeyInputComponent& keyInput = keyInputComps[i];
            const components::MouseInputComponent& mouseInput = mouseInputComps[i];

            if (mouseInput.hovered)
            {
                for (const auto& mouseEvent: mouseInput.mouseEvents)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        if (mouseEvent.state == input::KeyState::Up)
                        {
                            if (!text.inEditMode)
                            {
                                text.inEditMode = true;
                                text.editText = text.text;
                                text.caretPosition = 0;
                                keyInput.keyMask = static_cast<input::Key::Type>(0xFFFFFFFF);
                            }
                        }
                    }
                }
            }

            if (text.inEditMode)
            {
                for (const auto& keyEvent: keyInput.keyEvents)
                {
                    if (keyEvent.state == input::KeyState::Down)
                    {
                        if (keyEvent.key == input::Key::Escape)
                        {
                            text.inEditMode = false;
                            keyInput.keyMask = static_cast<input::Key::Type>(0);
                        }
                        else
                        {
                            HandleKey(text, keyInput, keyEvent.key);
                        }
                    }
                }
            }
        }
    }

    void EditableTextSystem::OnRender(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("EditableTextSystem::OnRender")

        auto app = Application::Get();
        auto renderer = render::Renderer::Get<render::Renderer>();
        auto window = app->GetPrimaryWindow();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        const auto& entities = updateData.GetEntities();
        const auto* widgetComps = updateData.GetComponentArray<const components::WidgetComponent>();
        const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* textComps = updateData.GetComponentArray<components::EditableTextComponent>();
        auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            const auto& widget = widgetComps[i];
            const auto& transform = transformComps[i];
            auto& text = textComps[i];

            util::RenderText(entity,
                             widget,
                             transform,
                             text,
                             windowsSize,
                             renderer,
                             renderComp,
                             text.inEditMode ? text.editText : text.text);
        }
    }
}
