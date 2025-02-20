#pragma once

#include "engine/reflect/Reflect.h"

namespace se::asset::texture
{
    DECLARE_SPARK_ENUM_BEGIN(Format, int)
        R8,
        DXT5,
        BC7
    DECLARE_SPARK_ENUM_END()
}