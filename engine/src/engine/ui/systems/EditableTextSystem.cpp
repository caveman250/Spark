#include "EditableTextSystem.h"

#include "UIKeyboardInputSystem.h"
#include "engine/input/InputComponent.h"
#include "engine/render/Renderer.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TextCaretComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/util/EditableTextUtil.h"
#include "engine/ui/util/TextUtil.h"
#include "platform/IWindow.h"
#include "platform/MouseCursorUtil.h"
#include "UIMouseInputSystem.h"

namespace se::ui::systems
{
    ecs::SystemDeclaration EditableTextSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Editable Text System")
                    .WithComponent<const components::RectTransformComponent>()
                    .WithComponent<components::EditableTextComponent>()
                    .WithComponent<components::WidgetComponent>()
                    .WithComponent<const components::MouseInputComponent>()
                    .WithComponent<components::KeyInputComponent>()
                    .WithSingletonComponent<singleton_components::UIRenderComponent>()
                    .WithSingletonComponent<const input::InputComponent>()
                    .WithHeirachyQuery<components::TextCaretComponent>()
                    .WithDependency<UIMouseInputSystem>()
                    .WithDependency<UIKeyboardInputSystem>();
    }

    char GetCharForKey(input::Key key, const input::InputComponent* inputComp)
    {
        bool shiftPressed = inputComp->keyStates[static_cast<int>(input::Key::LeftShift)] == input::KeyState::Down ||
            inputComp->keyStates[static_cast<int>(input::Key::RightShift)] == input::KeyState::Down;
        switch (key)
        {
            case input::Key::Unknown:
                return 0;
            case input::Key::Space:
                return ' ';
            case input::Key::Apostrophe:
                return shiftPressed ? '"' : '\'';
            case input::Key::Comma:
                return shiftPressed ? '<' : ',';
            case input::Key::Minus:
                return shiftPressed ? '_' : '-';
            case input::Key::Period:
                return shiftPressed ? '>' : '.';
            case input::Key::ForwardSlash:
                return shiftPressed ? '?' : '/';
            case input::Key::Key0:
                return shiftPressed ? ')' : '0';
            case input::Key::Key1:
                return shiftPressed ? '!' : '1';
            case input::Key::Key2:
                return shiftPressed ? '@' : '2';
            case input::Key::Key3:
                return shiftPressed ? '#' : '3';
            case input::Key::Key4:
                return shiftPressed ? '$' : '4';
            case input::Key::Key5:
                return shiftPressed ? '%' : '5';
            case input::Key::Key6:
                return shiftPressed ? '^' : '6';
            case input::Key::Key7:
                return shiftPressed ? '&' : '7';
            case input::Key::Key8:
                return shiftPressed ? '*' : '8';
            case input::Key::Key9:
                return shiftPressed ? '(' : '9';
            case input::Key::Semicolon:
                return shiftPressed ? ':' : ';';
            case input::Key::Equal:
                return shiftPressed ? '+' : '=';
            case input::Key::A:
                return shiftPressed ? 'A' : 'a';
            case input::Key::B:
                return shiftPressed ? 'B' : 'b';
            case input::Key::C:
                return shiftPressed ? 'C' : 'c';
            case input::Key::D:
                return shiftPressed ? 'D' : 'd';
            case input::Key::E:
                return shiftPressed ? 'E' : 'e';
            case input::Key::F:
                return shiftPressed ? 'F' : 'f';
            case input::Key::G:
                return shiftPressed ? 'G' : 'g';
            case input::Key::H:
                return shiftPressed ? 'H' : 'h';
            case input::Key::I:
                return shiftPressed ? 'I' : 'i';
            case input::Key::J:
                return shiftPressed ? 'J' : 'j';
            case input::Key::K:
                return shiftPressed ? 'K' : 'k';
            case input::Key::L:
                return shiftPressed ? 'L' : 'l';
            case input::Key::M:
                return shiftPressed ? 'M' : 'm';
            case input::Key::N:
                return shiftPressed ? 'N' : 'n';
            case input::Key::O:
                return shiftPressed ? 'O' : 'o';
            case input::Key::P:
                return shiftPressed ? 'P' : 'p';
            case input::Key::Q:
                return shiftPressed ? 'Q' : 'q';
            case input::Key::R:
                return shiftPressed ? 'R' : 'r';
            case input::Key::S:
                return shiftPressed ? 'S' : 's';
            case input::Key::T:
                return shiftPressed ? 'T' : 't';
            case input::Key::U:
                return shiftPressed ? 'U' : 'u';
            case input::Key::V:
                return shiftPressed ? 'V' : 'v';
            case input::Key::W:
                return shiftPressed ? 'W' : 'w';
            case input::Key::X:
                return shiftPressed ? 'X' : 'x';
            case input::Key::Y:
                return shiftPressed ? 'Y' : 'y';
            case input::Key::Z:
                return shiftPressed ? 'Z' : 'z';
            case input::Key::LeftBracket:
                return shiftPressed ? '{' : '[';
            case input::Key::Backslash:
                return shiftPressed ? '|' : '\\';
            case input::Key::RightBracket:
                return shiftPressed ? '}' : ']';
            case input::Key::GraveAccent:
                return shiftPressed ? '~' : '`';
            case input::Key::Num0:
                return shiftPressed ? 0 : '0';
            case input::Key::Num1:
                return shiftPressed ? 0 : '1';
            case input::Key::Num2:
                return shiftPressed ? 0 : '2';
            case input::Key::Num3:
                return shiftPressed ? 0 : '3';
            case input::Key::Num4:
                return shiftPressed ? 0 : '4';
            case input::Key::Num5:
                return shiftPressed ? 0 : '5';
            case input::Key::Num6:
                return shiftPressed ? 0 : '6';
            case input::Key::Num7:
                return shiftPressed ? 0 : '7';
            case input::Key::Num8:
                return shiftPressed ? 0 : '8';
            case input::Key::Num9:
                return shiftPressed ? 0 : '9';
            case input::Key::NumDecimal:
            case input::Key::NumDivide:
            case input::Key::NumMultiply:
            case input::Key::NumSubtract:
            case input::Key::NumAdd:
            case input::Key::NumEnter:
            case input::Key::NumEqual:
            case input::Key::World2:
            case input::Key::Escape:
            case input::Key::Enter:
            case input::Key::Tab:
            case input::Key::Backspace:
            case input::Key::Insert:
            case input::Key::Delete:
            case input::Key::Right:
            case input::Key::Left:
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
            case input::Key::LeftShift:
            case input::Key::LeftControl:
            case input::Key::LeftAlt:
            case input::Key::LeftSuper:
            case input::Key::RightShift:
            case input::Key::RightControl:
            case input::Key::RightAlt:
            case input::Key::RightSuper:
            case input::Key::Menu:
                return 0;
        }
        
        SPARK_ASSERT(false, "Unhandled key");
        return 0;
    }

    void AddChar(components::EditableTextComponent* text, input::Key key, const input::InputComponent* inputComp)
    {
        char c = GetCharForKey(key, inputComp);
        if (c != 0)
        {
            text->editText.insert(text->caretPosition, 1, c);
            util::MoveCaret(*text, 1);
        }
    }

    void EditableTextSystem::HandleKey([[maybe_unused]] const ecs::Id& entity,
                                       [[maybe_unused]] components::EditableTextComponent& text,
                                       [[maybe_unused]] components::KeyInputComponent& keyInput,
                                       [[maybe_unused]] input::Key key,
                                       [[maybe_unused]] const input::InputComponent* inputComponent)
    {
#if SPARK_EDITOR
        switch (key)
        {
            case input::Key::Space:
            case input::Key::Apostrophe:
            case input::Key::Comma:
            case input::Key::Minus:
            case input::Key::Period:
            case input::Key::ForwardSlash:
            case input::Key::Key0:
            case input::Key::Key1:
            case input::Key::Key2:
            case input::Key::Key3:
            case input::Key::Key4:
            case input::Key::Key5:
            case input::Key::Key6:
            case input::Key::Key7:
            case input::Key::Key8:
            case input::Key::Key9:
            case input::Key::Semicolon:
            case input::Key::Equal:
            case input::Key::A:
            case input::Key::B:
            case input::Key::C:
            case input::Key::D:
            case input::Key::E:
            case input::Key::F:
            case input::Key::G:
            case input::Key::H:
            case input::Key::I:
            case input::Key::J:
            case input::Key::K:
            case input::Key::L:
            case input::Key::M:
            case input::Key::N:
            case input::Key::O:
            case input::Key::P:
            case input::Key::Q:
            case input::Key::R:
            case input::Key::S:
            case input::Key::T:
            case input::Key::U:
            case input::Key::V:
            case input::Key::W:
            case input::Key::X:
            case input::Key::Y:
            case input::Key::Z:
            case input::Key::LeftBracket:
            case input::Key::Backslash:
            case input::Key::RightBracket:
            case input::Key::GraveAccent:
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
                AddChar(&text, key, inputComponent);
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
                text.editText.insert(text.caretPosition, "    ");
                util::MoveCaret(text, 4);
                break;
            case input::Key::Backspace:
                if (text.caretPosition > 0)
                {
                    text.editText.erase(text.caretPosition - 1, 1);
                    util::MoveCaret(text, -1);
                }
                break;
            case input::Key::Delete:
                if (text.caretPosition < static_cast<int>(text.editText.size()))
                {
                    text.editText.erase(text.caretPosition, 1);
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
#endif
    }

    void EditableTextSystem::OnUpdate([[maybe_unused]] const ecs::QueryResults& results)
    {
#if SPARK_EDITOR
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* textComps = updateData.GetComponentArray<components::EditableTextComponent>();
            const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
            const auto* mouseInputComps = updateData.GetComponentArray<const components::MouseInputComponent>();
            auto* keyInputComps = updateData.GetComponentArray<components::KeyInputComponent>();
            auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();
            const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                components::WidgetComponent& widget = widgets[i];

                if (!widget.updateEnabled || !widget.parentUpdateEnabled)
                {
                    continue;
                }

                const ecs::Id& entity = entities[i];
                components::EditableTextComponent& text = textComps[i];
                components::KeyInputComponent& keyInput = keyInputComps[i];
                const components::MouseInputComponent& mouseInput = mouseInputComps[i];
                const components::RectTransformComponent& rectTransform = transformComps[i];

                if (mouseInput.hovered)
                {
                    if (mouseInput.buttonMask != 0)
                    {
                        MouseCursorUtil::SetMouseCursor(MouseCursor::IBeam);

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
                }
                else if (inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down)
                {
                    util::EndEditingText(this, entity, text, keyInput);
                }

                if (!mouseInput.hovered && mouseInput.lastHovered)
                {
                    MouseCursorUtil::SetMouseCursor(MouseCursor::Arrow);
                }

                util::UpdateText(text, rectTransform, text.inEditMode ? text.editText : text.text);

                if (text.inEditMode)
                {
                    for (const auto& keyEvent: keyInput.keyEvents)
                    {
                        if (keyEvent.state == input::KeyState::Down)
                        {
                            if (keyEvent.key == input::Key::Escape)
                            {
                                util::EndEditingText(this, entity, text, keyInput);
                                break;
                            }
                            else
                            {
                                HandleKey(entity, text, keyInput, keyEvent.key, inputComp);
                            }
                        }
                    }

                    if (text.inEditMode)
                    {
                        auto window = Application::Get()->GetWindow();
                        constexpr float caretWidth = 2.f;
                        float offset = (util::GetCaretPosition(text.caretPosition, text)) * window->GetContentScale();
                        if (offset > 0.f)
                        {
                            //account for caret size
                            offset += caretWidth * window->GetContentScale();
                        }
                        float localOffset = offset + text.renderOffset;
                        if (localOffset > rectTransform.rect.size.x)
                        {
                            text.renderOffset = rectTransform.rect.size.x - offset;
                            util::SetCaretPos(text, text.caretPosition);
                        }
                        else if (localOffset < 0)
                        {
                            text.renderOffset = -offset;
                            util::SetCaretPos(text, text.caretPosition);
                        }
                    }
                }
            }
        });
#endif
    }

    void EditableTextSystem::OnRender([[maybe_unused]] const ecs::QueryResults& results)
    {
#if SPARK_EDITOR
        EASY_BLOCK("EditableTextSystem::OnRender");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            auto app = Application::Get();
            auto renderer = render::Renderer::Get<render::Renderer>();
            auto window = app->GetWindow();

            const auto& entities = updateData.GetEntities();
            auto* widgetComps = updateData.GetComponentArray<components::WidgetComponent>();
            const auto* transformComps = updateData.GetComponentArray<const components::RectTransformComponent>();
            auto* textComps = updateData.GetComponentArray<components::EditableTextComponent>();
            auto* renderComp = updateData.GetSingletonComponent<singleton_components::UIRenderComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& widget = widgetComps[i];
                const auto& entity = entities[i];
                const auto& transform = transformComps[i];
                auto& text = textComps[i];

                auto windowSize = ecs::IsEditorEntity(entity) ?
                    math::IntVec2(window->GetWidth(), window->GetHeight()) :
                    Application::Get()->GetGameViewportSize();

                util::RenderText(entity,
                                 widget,
                                 transform,
                                 text,
                                 windowSize,
                                 renderer,
                                 renderComp,
                                 text.inEditMode ? text.editText : text.text,
                                 { text.renderOffset, 0.f });
            }
        });
#endif
    }
}
