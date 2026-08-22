module;

#include "spark.h"

export module Spark.Asset.Texture.Format;

namespace se::asset::texture
{
    SPARK_ENUM()
    export enum class Format
    {
        R8,
        BGRA8,
        RGBA8,
        Depth16,
        DXT5,
        BC7
    };

    SPARK_ENUM()
    export enum class Usage
    {
        Read,
        RenderTarget,
        Read_RenderTarget,
    };

    export bool IsCompressedFormat(Format format);
    export size_t GetCompressedFormatBlockSize(Format format);
    export size_t GetCompressedFormatBlockSizeBytes(Format format);
    export size_t GetNumTextureChannels(Format format);
}
