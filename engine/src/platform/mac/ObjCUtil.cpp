#include "ObjCUtil.h"

#import <Cocoa/Cocoa.h>

#include "KeyMap.h"
#include "engine/input/Key.h"
#include "platform/mac/Window.h"
#include <stdint.h>

#include "engine/input/InputComponent.h"
#include "engine/input/KeyEvent.h"


import Application;
import MaterialInstance;
import Material;

namespace se::platform::mac
{
    NSWindowPtr GetNativeWindow()
    {
        auto* window = Application::Get()->GetWindow();
        return static_cast<se::mac::Window*>(window)->GetNativeWindow();
    }
    int GetWindowRect()
    {
        auto* window = Application::Get()->GetWindow();
        return window->GetWidth();
    }
    int GetWindowHeight()
    {
        auto* window = Application::Get()->GetWindow();
        return window->GetHeight();
    }

    void OnWindowResize(int width, int height)
    {
        if (auto window = se::Application::Get()->GetWindow())
        {
            window->OnResize(width / window->GetContentScale(), height / window->GetContentScale());
        }
    }

    SparkViewPtr GetView()
    {
        auto* window = static_cast<se::platform::mac::Window*>(Application::Get()->GetWindow());
        return window->GetView();
    }

    void OnLeftMouseDown()
    {
        OnMouseDown(input::MouseButton::Left);
    }

    void OnRightMouseDown()
    {
        OnMouseDown(input::MouseButton::Right);
    }

    void OnLeftMouseUp()
    {
        OnMouseUp(input::MouseButton::Left);
    }

    void OnRightMouseUp()
    {
        OnMouseUp(input::MouseButton::Right);
    }

    void OnMouseDown(input::MouseButton button)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
        inputComp->mouseButtonStates[static_cast<int>(button)] = se::input::KeyState::Down;

        se::input::MouseEvent mouseEvent;
        mouseEvent.button = button;
        mouseEvent.state = se::input::KeyState::Down;
        inputComp->mouseEvents.push_back(mouseEvent);
    }

    void OnMouseUp(input::MouseButton button)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
        inputComp->mouseButtonStates[static_cast<int>(button)] = se::input::KeyState::Up;

        se::input::MouseEvent mouseEvent;
        mouseEvent.button = button;
        mouseEvent.state = se::input::KeyState::Up;
        inputComp->mouseEvents.push_back(mouseEvent);
    }

    void OnMouseScroll(float delta)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
        inputComp->mouseScrollDelta = delta;

        se::input::MouseEvent mouseEvent;
        mouseEvent.button = se::input::MouseButton::None;
        mouseEvent.scrollDelta = -inputComp->mouseScrollDelta;
        inputComp->mouseEvents.push_back(mouseEvent);
    }

    void OnMouseMove(int x,
        int y)
    {
        auto app = se::Application::Get();
        auto window = app->GetWindow();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
        inputComp->mouseX = x * window->GetContentScale();
        inputComp->mouseY = y * window->GetContentScale();
    }

    void OnMouseDragged(int x,
        int y)
    {
        auto app = se::Application::Get();
        auto window = app->GetWindow();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

        OnMouseMove(x, y);

        if (inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] != se::input::KeyState::Down)
        {
            inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] = se::input::KeyState::Down;

            se::input::MouseEvent mouseEvent;
            mouseEvent.button = se::input::MouseButton::Left;
            mouseEvent.state = se::input::KeyState::Down;
            inputComp->mouseEvents.push_back(mouseEvent);
        }
    }

    void OnRightMouseDragged(int x,
        int y)
    {
        auto app = se::Application::Get();
        auto window = app->GetWindow();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

        if (inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Right)] != se::input::KeyState::Down)
        {
            inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Right)] = se::input::KeyState::Down;

            se::input::MouseEvent mouseEvent;
            mouseEvent.button = se::input::MouseButton::Right;
            mouseEvent.state = se::input::KeyState::Down;
            inputComp->mouseEvents.push_back(mouseEvent);
        }

        inputComp->mouseX = x * window->GetContentScale();
        inputComp->mouseY = y * window->GetContentScale();
    }

    // void BindMaterialInstance(const std::shared_ptr<render::MaterialInstance>& instance,
    //     const std::shared_ptr<render::VertexBuffer>& vertBuffer)
    // {
    //     instance->Bind(*vertBuffer);
    // }
    //
    // render::DrawMode GetDrawMode(const std::shared_ptr<render::MaterialInstance>& instance)
    // {
    //     return instance->GetMaterial()->GetRenderState().drawMode;
    // }

    // const std::map<std::string, asset::SerializedVariable>& GetVertUniforms(render::Material* mat)
    // {
    //     return mat->GetVertUniforms();
    // }
    //
    // const std::map<std::string, asset::SerializedVariable>& GetFragUniforms(render::Material* mat)
    // {
    //     return mat->GetFragUniforms();
    // }

    enum NSEventModifierFlags
    {
        NSEventModifierFlagCapsLock           = 1 << 16, // Set if Caps Lock key is pressed.
        NSEventModifierFlagShift              = 1 << 17, // Set if Shift key is pressed.
        NSEventModifierFlagControl            = 1 << 18, // Set if Control key is pressed.
        NSEventModifierFlagOption             = 1 << 19, // Set if Option or Alternate key is pressed.
        NSEventModifierFlagCommand            = 1 << 20, // Set if Command key is pressed.
        NSEventModifierFlagNumericPad         = 1 << 21, // Set if any key in the numeric keypad is pressed.
        NSEventModifierFlagHelp               = 1 << 22, // Set if the Help key is pressed.
        NSEventModifierFlagFunction           = 1 << 23, // Set if any function key is pressed.
    };

    void OnKeyStateChange(uint32_t key,
        bool modifierFlagsChanged,
        uint32_t modifierFlags,
        bool keyDown)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

        se::input::Key key = se::mac::KeyMap::MacKeyToSparkKey(key);
        if (modifierFlagsChanged)
        {
            switch (key)
            {
                case se::input::Key::LeftSuper:
                case se::input::Key::RightSuper:
                    keyDown = modifierFlags & NSEventModifierFlagCommand;
                    break;
                case se::input::Key::LeftShift:
                case se::input::Key::RightShift:
                    keyDown = modifierFlags & NSEventModifierFlagShift;
                    break;
                case se::input::Key::LeftControl:
                case se::input::Key::RightControl:
                    keyDown = modifierFlags & NSEventModifierFlagControl;
                    break;
                case se::input::Key::LeftAlt:
                case se::input::Key::RightAlt:
                    keyDown = modifierFlags & NSEventModifierFlagOption;
                    break;
                default:
                    return;
            }

            se::input::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.state = keyDown ? se::input::KeyState::Down : se::input::KeyState::Up;

            inputComp->keyEvents.push_back(keyEvent);
            inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
        }
        else
        {
            inputComp->keyStates[static_cast<int>(key)] = keyDown ?
                     se::input::KeyState::Down :
                     se::input::KeyState::Up;

            se::input::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.state = keyDown ? se::input::KeyState::Down : se::input::KeyState::Up;
            inputComp->keyEvents.push_back(keyEvent);
        }
    }
}
