#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ui::text
{
    SPARK_ENUM()
    enum class WrapMode : int
    {
        Word,
        Char,
        WordChar,
        None
    };
}
