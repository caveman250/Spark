#include "KeyUtil.h"

namespace se::input::util
{
    bool IsModifierKey(Key::Type key)
    {
        static int modifierMask = Key::LeftShift |
                                  Key::RightShift |
                                  Key::LeftControl |
                                  Key::RightControl |
                                  Key::LeftAlt |
                                  Key::RightAlt |
                                  Key::LeftSuper |
                                  Key::RightSuper;
        return (key & modifierMask) != 0;
    }
}
