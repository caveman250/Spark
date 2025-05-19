#include "KeyMap.h"

namespace se::linux
{
    input::Key::Type KeyMap::SDLKeyToSparkKey(SDL_Scancode sdlKey)
    {
        switch (sdlKey)
        {
        case SDL_SCANCODE_UNKNOWN: return input::Key::Unknown;
        case  SDL_SCANCODE_A: return input::Key::A;
        case  SDL_SCANCODE_B: return input::Key::B;
        case  SDL_SCANCODE_C: return input::Key::C;
        case  SDL_SCANCODE_D: return input::Key::D;
        case  SDL_SCANCODE_E: return input::Key::E;
        case  SDL_SCANCODE_F: return input::Key::F;
        case  SDL_SCANCODE_G: return input::Key::G;
        case  SDL_SCANCODE_H: return input::Key::H;
        case  SDL_SCANCODE_I: return input::Key::I;
        case  SDL_SCANCODE_J: return input::Key::J;
        case  SDL_SCANCODE_K: return input::Key::K;
        case  SDL_SCANCODE_L: return input::Key::L;
        case  SDL_SCANCODE_M: return input::Key::M;
        case  SDL_SCANCODE_N: return input::Key::N;
        case  SDL_SCANCODE_O: return input::Key::O;
        case  SDL_SCANCODE_P: return input::Key::P;
        case  SDL_SCANCODE_Q: return input::Key::Q;
        case  SDL_SCANCODE_R: return input::Key::R;
        case  SDL_SCANCODE_S: return input::Key::S;
        case  SDL_SCANCODE_T: return input::Key::T;
        case  SDL_SCANCODE_U: return input::Key::U;
        case  SDL_SCANCODE_V: return input::Key::V;
        case  SDL_SCANCODE_W: return input::Key::W;
        case  SDL_SCANCODE_X: return input::Key::X;
        case  SDL_SCANCODE_Y: return input::Key::Y;
        case  SDL_SCANCODE_Z: return input::Key::Z;
        case  SDL_SCANCODE_1: return input::Key::Key1;
        case  SDL_SCANCODE_2: return input::Key::Key2;
        case  SDL_SCANCODE_3: return input::Key::Key3;
        case  SDL_SCANCODE_4: return input::Key::Key4;
        case  SDL_SCANCODE_5: return input::Key::Key5;
        case  SDL_SCANCODE_6: return input::Key::Key6;
        case  SDL_SCANCODE_7: return input::Key::Key7;
        case  SDL_SCANCODE_8: return input::Key::Key8;
        case  SDL_SCANCODE_9: return input::Key::Key9;
        case  SDL_SCANCODE_0: return input::Key::Key0;
        case  SDL_SCANCODE_RETURN: return input::Key::Enter;
        case  SDL_SCANCODE_ESCAPE: return input::Key::Escape;
        case  SDL_SCANCODE_BACKSPACE: return input::Key::Backspace;
        case  SDL_SCANCODE_TAB: return input::Key::Tab;
        case  SDL_SCANCODE_SPACE: return input::Key::Space;
        case  SDL_SCANCODE_MINUS: return input::Key::Minus;
        case  SDL_SCANCODE_EQUALS: return input::Key::Equal;
        case  SDL_SCANCODE_LEFTBRACKET: return input::Key::LeftBracket;
        case  SDL_SCANCODE_RIGHTBRACKET: return input::Key::RightBracket;
        case  SDL_SCANCODE_BACKSLASH: return input::Key::Backslash;
        case  SDL_SCANCODE_SEMICOLON: return input::Key::Semicolon;
        case  SDL_SCANCODE_APOSTROPHE: return input::Key::Apostrophe;
        case  SDL_SCANCODE_GRAVE: return input::Key::GraveAccent;
        case  SDL_SCANCODE_COMMA: return input::Key::Comma;
        case  SDL_SCANCODE_PERIOD: return input::Key::Period;
        case  SDL_SCANCODE_SLASH: return input::Key::ForwardSlash;
        case  SDL_SCANCODE_CAPSLOCK: return input::Key::CapsLock;
        case  SDL_SCANCODE_F1: return input::Key::F1;
        case  SDL_SCANCODE_F2: return input::Key::F2;
        case  SDL_SCANCODE_F3: return input::Key::F3;
        case  SDL_SCANCODE_F4: return input::Key::F4;
        case  SDL_SCANCODE_F5: return input::Key::F5;
        case  SDL_SCANCODE_F6: return input::Key::F6;
        case  SDL_SCANCODE_F7: return input::Key::F7;
        case  SDL_SCANCODE_F8: return input::Key::F8;
        case  SDL_SCANCODE_F9: return input::Key::F9;
        case  SDL_SCANCODE_F10: return input::Key::F10;
        case  SDL_SCANCODE_F11: return input::Key::F11;
        case  SDL_SCANCODE_F12: return input::Key::F12;
        case  SDL_SCANCODE_PRINTSCREEN: return input::Key::PrintScreen;
        case  SDL_SCANCODE_SCROLLLOCK: return input::Key::ScrollLock;
        case  SDL_SCANCODE_PAUSE: return input::Key::Pause;
        case  SDL_SCANCODE_INSERT: return input::Key::Insert;
        case  SDL_SCANCODE_HOME: return input::Key::Home;
        case  SDL_SCANCODE_PAGEUP: return input::Key::PageUp;
        case  SDL_SCANCODE_DELETE: return input::Key::Delete;
        case  SDL_SCANCODE_END: return input::Key::End;
        case  SDL_SCANCODE_PAGEDOWN: return input::Key::PageDown;
        case  SDL_SCANCODE_RIGHT: return input::Key::Right;
        case  SDL_SCANCODE_LEFT: return input::Key::Left;
        case  SDL_SCANCODE_DOWN: return input::Key::Down;
        case  SDL_SCANCODE_UP: return input::Key::Up;
        case  SDL_SCANCODE_NUMLOCKCLEAR: return input::Key::NumLock;
        case  SDL_SCANCODE_KP_DIVIDE: return input::Key::NumDivide;
        case  SDL_SCANCODE_KP_MULTIPLY: return input::Key::NumMultiply;
        case  SDL_SCANCODE_KP_ENTER: return input::Key::NumEnter;
        case  SDL_SCANCODE_KP_1: return input::Key::Num1;
        case  SDL_SCANCODE_KP_2: return input::Key::Num2;
        case  SDL_SCANCODE_KP_3: return input::Key::Num3;
        case  SDL_SCANCODE_KP_4: return input::Key::Num4;
        case  SDL_SCANCODE_KP_5: return input::Key::Num5;
        case  SDL_SCANCODE_KP_6: return input::Key::Num6;
        case  SDL_SCANCODE_KP_7: return input::Key::Num7;
        case  SDL_SCANCODE_KP_8: return input::Key::Num8;
        case  SDL_SCANCODE_KP_9: return input::Key::Num9;
        case  SDL_SCANCODE_KP_0: return input::Key::Num0;
        case  SDL_SCANCODE_KP_PERIOD: return input::Key::Period;
        case  SDL_SCANCODE_NONUSBACKSLASH: return input::Key::Backslash;
        case  SDL_SCANCODE_KP_EQUALS: return input::Key::NumEqual;
        case  SDL_SCANCODE_F13: return input::Key::F13;
        case  SDL_SCANCODE_F14: return input::Key::F14;
        case  SDL_SCANCODE_F15: return input::Key::F15;
        case  SDL_SCANCODE_F16: return input::Key::F16;
        case  SDL_SCANCODE_F17: return input::Key::F17;
        case  SDL_SCANCODE_F18: return input::Key::F18;
        case  SDL_SCANCODE_F19: return input::Key::F19;
        case  SDL_SCANCODE_F20: return input::Key::F20;
        case  SDL_SCANCODE_F21: return input::Key::F21;
        case  SDL_SCANCODE_F22: return input::Key::F22;
        case  SDL_SCANCODE_F23: return input::Key::F23;
        case  SDL_SCANCODE_F24: return input::Key::F24;
        case  SDL_SCANCODE_MENU: return input::Key::Menu;
        case  SDL_SCANCODE_LCTRL: return input::Key::LeftControl;
        case  SDL_SCANCODE_LSHIFT: return input::Key::LeftShift;
        case  SDL_SCANCODE_LALT: return input::Key::LeftAlt;
        case  SDL_SCANCODE_RCTRL: return input::Key::RightControl;
        case  SDL_SCANCODE_RSHIFT: return input::Key::RightShift;
        case  SDL_SCANCODE_RALT: return input::Key::RightAlt;
        default:
            SPARK_ASSERT(false, "KeyMap.h::SDLKeyToSparkKey - Unknown SDL key: {}.", static_cast<int>(sdlKey));
            return input::Key::Unknown;
        }
    }
}
