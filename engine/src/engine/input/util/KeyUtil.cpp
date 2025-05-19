#include "KeyUtil.h"

namespace se::input::util
{
    bool IsModifierKey(Key key)
    {
        static int modifierMask = static_cast<int>(Key::LeftShift) |
                                  static_cast<int>(Key::RightShift) |
                                  static_cast<int>(Key::LeftControl) |
                                  static_cast<int>(Key::RightControl) |
                                  static_cast<int>(Key::LeftAlt) |
                                  static_cast<int>(Key::RightAlt) |
                                  static_cast<int>(Key::LeftSuper) |
                                  static_cast<int>(Key::RightSuper);
        return (static_cast<int>(key) & modifierMask) != 0;
    }
}
