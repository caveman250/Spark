#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ui::text
{
    SPARK_ENUM_BEGIN(WrapMode, int)
        Word,
        Char,
        WordChar,
        None
    SPARK_ENUM_END()
}
