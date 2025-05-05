#pragma once

#include "engine/reflect/Reflect.h"

namespace se::asset::texture
{
    DECLARE_SPARK_ENUM_BEGIN(Format, int)
        R8,
        BGRA8,
        RGBA8,
        Depth16,
        DXT5,
        BC7
    DECLARE_SPARK_ENUM_END()

    DECLARE_SPARK_ENUM_BEGIN(Usage, int)
        Read,
        RenderTarget,
        Read_RenderTarget,
    DECLARE_SPARK_ENUM_END()

    bool IsCompressedFormat(Format::Type format);
    size_t GetCompressedFormatBlockSize(Format::Type format);
    size_t GetCompressedFormatBlockSizeBytes(Format::Type format);
    size_t GetNumTextureChannels(Format::Type format);
}
