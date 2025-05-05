#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ui::text
{
    DECLARE_SPARK_ENUM_BEGIN(WrapMode, int)
        Word,
        Char,
        None
    DECLARE_SPARK_ENUM_END()
}
