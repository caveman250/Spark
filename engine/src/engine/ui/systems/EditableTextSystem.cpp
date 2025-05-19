#include "EditableTextSystem.h"

#include "engine/input/InputComponent.h"
#include "engine/render/Renderer.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/util/EditableTextUtil.h"
#include "engine/ui/util/TextUtil.h"
#include "platform/IWindow.h"

namespace se::ui::systems
{
    void EditableTextSystem::HandleKey(const ecs::Id& entity,
                                       components::EditableTextComponent& text,
                                       components::KeyInputComponent& keyInput,
                                       input::Key key)
    {
        switch (key)
        {
            case input::Key::Space:
                text.editText.Insert(text.caretPosition, ' ');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Apostrophe:
                text.editText.Insert(text.caretPosition, '\'');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Comma:
                text.editText.Insert(text.caretPosition, ',');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Minus:
                text.editText.Insert(text.caretPosition, '-');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Period:
                text.editText.Insert(text.caretPosition, '.');
                util::MoveCaret(text, 1);
                break;
            case input::Key::ForwardSlash:
                text.editText.Insert(text.caretPosition, '/');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key0:
                text.editText.Insert(text.caretPosition, '0');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key1:
                text.editText.Insert(text.caretPosition, '1');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key2:
                text.editText.Insert(text.caretPosition, '2');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key3:
                text.editText.Insert(text.caretPosition, '3');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key4:
                text.editText.Insert(text.caretPosition, '4');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key5:
                text.editText.Insert(text.caretPosition, '5');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key6:
                text.editText.Insert(text.caretPosition, '6');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key7:
                text.editText.Insert(text.caretPosition, '7');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key8:
                text.editText.Insert(text.caretPosition, '8');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Key9:
                text.editText.Insert(text.caretPosition, '9');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Semicolon:
                text.editText.Insert(text.caretPosition, ';');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Equal:
                text.editText.Insert(text.caretPosition, '=');
                util::MoveCaret(text, 1);
                break;
            case input::Key::A:
                text.editText.Insert(text.caretPosition, 'a');
                util::MoveCaret(text, 1);
                break;
            case input::Key::B:
                text.editText.Insert(text.caretPosition, 'b');
                util::MoveCaret(text, 1);
                break;
            case input::Key::C:
                text.editText.Insert(text.caretPosition, 'c');
                util::MoveCaret(text, 1);
                break;
            case input::Key::D:
                text.editText.Insert(text.caretPosition, 'd');
                util::MoveCaret(text, 1);
                break;
            case input::Key::E:
                text.editText.Insert(text.caretPosition, 'e');
                util::MoveCaret(text, 1);
                break;
            case input::Key::F:
                text.editText.Insert(text.caretPosition, 'f');
                util::MoveCaret(text, 1);
                break;
            case input::Key::G:
                text.editText.Insert(text.caretPosition, 'g');
                util::MoveCaret(text, 1);
                break;
            case input::Key::H:
                text.editText.Insert(text.caretPosition, 'h');
                util::MoveCaret(text, 1);
                break;
            case input::Key::I:
                text.editText.Insert(text.caretPosition, 'i');
                util::MoveCaret(text, 1);
                break;
            case input::Key::J:
                text.editText.Insert(text.caretPosition, 'j');
                util::MoveCaret(text, 1);
                break;
            case input::Key::K:
                text.editText.Insert(text.caretPosition, 'k');
                util::MoveCaret(text, 1);
                break;
            case input::Key::L:
                text.editText.Insert(text.caretPosition, 'l');
                util::MoveCaret(text, 1);
                break;
            case input::Key::M:
                text.editText.Insert(text.caretPosition, 'm');
                util::MoveCaret(text, 1);
                break;
            case input::Key::N:
                text.editText.Insert(text.caretPosition, 'n');
                util::MoveCaret(text, 1);
                break;
            case input::Key::O:
                text.editText.Insert(text.caretPosition, 'o');
                util::MoveCaret(text, 1);
                break;
            case input::Key::P:
                text.editText.Insert(text.caretPosition, 'p');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Q:
                text.editText.Insert(text.caretPosition, 'q');
                util::MoveCaret(text, 1);
                break;
            case input::Key::R:
                text.editText.Insert(text.caretPosition, 'r');
                util::MoveCaret(text, 1);
                break;
            case input::Key::S:
                text.editText.Insert(text.caretPosition, 's');
                util::MoveCaret(text, 1);
                break;
            case input::Key::T:
                text.editText.Insert(text.caretPosition, 't');
                util::MoveCaret(text, 1);
                break;
            case input::Key::U:
                text.editText.Insert(text.caretPosition, 'u');
                util::MoveCaret(text, 1);
                break;
            case input::Key::V:
                text.editText.Insert(text.caretPosition, 'v');
                util::MoveCaret(text, 1);
                break;
            case input::Key::W:
                text.editText.Insert(text.caretPosition, 'w');
                util::MoveCaret(text, 1);
                break;
            case input::Key::X:
                text.editText.Insert(text.caretPosition, 'x');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Y:
                text.editText.Insert(text.caretPosition, 'y');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Z:
                text.editText.Insert(text.caretPosition, 'z');
                util::MoveCaret(text, 1);
                break;
            case input::Key::LeftBracket:
                text.editText.Insert(text.caretPosition, '[');
                util::MoveCaret(text, 1);
                break;
            case input::Key::Backslash:
                text.editText.Insert(text.caretPosition, '\\');
                util::MoveCaret(text, 1);
                break;
            case input::Key::RightBracket:
                text.editText.Insert(text.caretPosition, ']');
                util::MoveCaret(text, 1);
                break;
            case input::Key::GraveAccent:
                text.editText.Insert(text.caretPosition, '`');
                util::MoveCaret(text, 1);
                break;
            case input::Key::World2:
                break;
            case input::Key::Escape:
                break;
            case input::Key::Enter:
                text.onComitted.Broadcast(text.editText);
                text.text = text.editText;
                util::EndEditingText(this, entity, text, keyInput);
                break;
            case input::Key::Tab:
                text.editText.Insert(text.caretPosition, "    ");
                break;
            case input::Key::Backspace:
                if (text.caretPosition > 0)
                {
                    text.editText.Erase(text.caretPosition - 1);
                    util::MoveCaret(text, -1);
                }
                break;
            case input::Key::Delete:
                if (text.caretPosition < static_cast<int>(text.editText.Size()))
                {
                    text.editText.Erase(text.caretPosition);
                }
                break;
            case input::Key::Right:
                util::MoveCaret(text, 1);
                break;
            case input::Key::Left:
                util::MoveCaret(text, -1);
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
        const auto& entities = updateData.GetEntities();
        auto* textComps = updateData.GetComponentArray<components::EditableTextComponent>();
        const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
        const auto* mouseInputComps = updateData.GetComponentArray<const components::MouseInputComponent>();
        auto* keyInputComps = updateData.GetComponentArray<components::KeyInputComponent>();
        const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const ecs::Id& entity = entities[i];
            components::EditableTextComponent& text = textComps[i];
            components::KeyInputComponent& keyInput = keyInputComps[i];
            const components::MouseInputComponent& mouseInput = mouseInputComps[i];
            const components::RectTransformComponent& rectTransform = transformComps[i];

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
                                util::BeginEditingText(this, entity, text, keyInput);
                            }

                            util::SetCaretPos(text,
                                              util::CalcCaretPosition(
                                                  math::Vec2(static_cast<float>(inputComp->mouseX), static_cast<float>(inputComp->mouseY)),
                                                  text,
                                                  rectTransform));
                        }
                    }
                }
            }
            else if (text.inEditMode)
            {
                util::EndEditingText(this, entity, text, keyInput);
            }

            if (text.inEditMode)
            {
                for (const auto& keyEvent: keyInput.keyEvents)
                {
                    if (keyEvent.state == input::KeyState::Down)
                    {
                        if (keyEvent.key == input::Key::Escape)
                        {
                            util::EndEditingText(this, entity, text, keyInput);
                        }
                        else
                        {
                            HandleKey(entity, text, keyInput, keyEvent.key);
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
        auto* widgetComps = updateData.GetComponentArray<components::WidgetComponent>();
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
