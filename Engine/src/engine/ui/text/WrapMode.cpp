#include "WrapMode.h"

DEFINE_SPARK_ENUM_BEGIN(se::ui::text::WrapMode)
    DEFINE_ENUM_VALUE(WrapMode, Word)
    DEFINE_ENUM_VALUE(WrapMode, Char)
    DEFINE_ENUM_VALUE(WrapMode, WordChar)
    DEFINE_ENUM_VALUE(WrapMode, None)
DEFINE_SPARK_ENUM_END()