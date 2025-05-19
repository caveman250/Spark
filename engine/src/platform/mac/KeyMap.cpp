#include "KeyMap.h"

namespace se::mac
{
    input::Key KeyMap::MacKeyToSparkKey(uint32_t macKey)
    {
        switch (macKey)
        {
            case 0x31: return input::Key::Space;
            case 0x27: return input::Key::Apostrophe;
            case 0x2B: return input::Key::Comma;
            case 0x1B: return input::Key::Minus;
            case 0x2F: return input::Key::Period;
            case 0x2C: return input::Key::ForwardSlash;
            case 0x1D: return input::Key::Key0;
            case 0x12: return input::Key::Key1;
            case 0x13: return input::Key::Key2;
            case 0x14: return input::Key::Key3;
            case 0x15: return input::Key::Key4;
            case 0x17: return input::Key::Key5;
            case 0x16: return input::Key::Key6;
            case 0x1A: return input::Key::Key7;
            case 0x1C: return input::Key::Key8;
            case 0x19: return input::Key::Key9;
            case 0x29: return input::Key::Semicolon;
            case 0x18: return input::Key::Equal;
            case 0x00: return input::Key::A;
            case 0x0B: return input::Key::B;
            case 0x08: return input::Key::C;
            case 0x02: return input::Key::D;
            case 0x0E: return input::Key::E;
            case 0x03: return input::Key::F;
            case 0x05: return input::Key::G;
            case 0x04: return input::Key::H;
            case 0x22: return input::Key::I;
            case 0x26: return input::Key::J;
            case 0x28: return input::Key::K;
            case 0x25: return input::Key::L;
            case 0x2E: return input::Key::M;
            case 0x2D: return input::Key::N;
            case 0x1F: return input::Key::O;
            case 0x23: return input::Key::P;
            case 0x0C: return input::Key::Q;
            case 0x0F: return input::Key::R;
            case 0x01: return input::Key::S;
            case 0x11: return input::Key::T;
            case 0x20: return input::Key::U;
            case 0x09: return input::Key::V;
            case 0x0D: return input::Key::W;
            case 0x07: return input::Key::X;
            case 0x10: return input::Key::Y;
            case 0x06: return input::Key::Z;
            case 0x21: return input::Key::LeftBracket;
            case 0x2A: return input::Key::Backslash;
            case 0x1E: return input::Key::RightBracket;
            case 0x32: return input::Key::GraveAccent;
            case 0x35: return input::Key::Escape;
            case 0x24: return input::Key::Enter;
            case 0x30: return input::Key::Tab;
            case 0x33: return input::Key::Backspace;
            case 0x7C: return input::Key::Right;
            case 0x7B: return input::Key::Left;
            case 0x7D: return input::Key::Down;
            case 0x7E: return input::Key::Up;
            case 0x74: return input::Key::PageUp;
            case 0x79: return input::Key::PageDown;
            case 0x73: return input::Key::Home;
            case 0x77: return input::Key::End;
            case 0x39: return input::Key::CapsLock;
            case 0x47: return input::Key::NumLock;
            case 0x7A: return input::Key::F1;
            case 0x78: return input::Key::F2;
            case 0x63: return input::Key::F3;
            case 0x76: return input::Key::F4;
            case 0x60: return input::Key::F5;
            case 0x61: return input::Key::F6;
            case 0x62: return input::Key::F7;
            case 0x64: return input::Key::F8;
            case 0x65: return input::Key::F9;
            case 0x6D: return input::Key::F10;
            case 0x67: return input::Key::F11;
            case 0x6F: return input::Key::F12;
            case 0x69: return input::Key::F13;
            case 0x6B: return input::Key::F14;
            case 0x71: return input::Key::F15;
            case 0x6A: return input::Key::F16;
            case 0x40: return input::Key::F17;
            case 0x4F: return input::Key::F18;
            case 0x50: return input::Key::F19;
            case 0x5A: return input::Key::F20;
            case 0x52: return input::Key::Num0;
            case 0x53: return input::Key::Num1;
            case 0x54: return input::Key::Num2;
            case 0x55: return input::Key::Num3;
            case 0x56: return input::Key::Num4;
            case 0x57: return input::Key::Num5;
            case 0x58: return input::Key::Num6;
            case 0x59: return input::Key::Num7;
            case 0x5B: return input::Key::Num8;
            case 0x5C: return input::Key::Num9;
            case 0x41: return input::Key::NumDecimal;
            case 0x4B: return input::Key::NumDivide;
            case 0x43: return input::Key::NumMultiply;
            case 0x4E: return input::Key::NumSubtract;
            case 0x45: return input::Key::NumAdd;
            case 0x4C: return input::Key::NumEnter;
            case 0x51: return input::Key::NumEqual;
            case 0x38: return input::Key::LeftShift;
            case 0x3B: return input::Key::LeftControl;
            case 0x3A: return input::Key::LeftAlt;
            case 0x37: return input::Key::LeftSuper;
            case 0x3C: return input::Key::RightShift;
            case 0x3E: return input::Key::RightControl;
            case 0x3D: return input::Key::RightAlt;
            case 0x36: return input::Key::RightSuper;
            default: return input::Key::Unknown;
        }
    }
}

