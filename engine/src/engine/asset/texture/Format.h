#pragma once

#include "engine/reflect/Reflect.h"

namespace se::asset::texture
{
    SPARK_ENUM()
    enum class Format
    {
        R8,
        BGRA8,
        RGBA8,
        Depth16,
        DXT5,
        BC7
    };

    SPARK_ENUM()
    enum class Usage
    {
        Read,
        RenderTarget,
        Read_RenderTarget,
    };

    bool IsCompressedFormat(Format format);
    size_t GetCompressedFormatBlockSize(Format format);
    size_t GetCompressedFormatBlockSizeBytes(Format format);
    size_t GetNumTextureChannels(Format format);
}
