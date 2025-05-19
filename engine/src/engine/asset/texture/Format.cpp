#include "Format.h"

namespace se::asset::texture
{
    bool IsCompressedFormat(Format format)
    {
        switch (format)
        {
            case Format::R8:
            case Format::BGRA8:
            case Format::RGBA8:
            case Format::Depth16:
                return false;
            case Format::DXT5:
            case Format::BC7:
                return true;
            default:
                SPARK_ASSERT(false, "IsCompressedFormat - Unhandled format");
        }

        return false;
    }

    size_t GetCompressedFormatBlockSize(Format format)
    {
        switch (format)
        {
            case Format::DXT5:
                return 4;
            case Format::BC7:
                return 4;
            default:
                SPARK_ASSERT(false, "GetCompressedFormatBlockSize - Unhandled format");
                break;
        }

        return 0;
    }

    size_t GetCompressedFormatBlockSizeBytes(Format format)
    {
        switch (format)
        {
            case Format::DXT5:
                return 16;
            case Format::BC7:
                return 16;
            default:
                SPARK_ASSERT(false, "GetCompressedFormatBlockSizeBytes - Unhandled Format");
        }

        return 0;
    }

    size_t GetNumTextureChannels(Format format)
    {
        switch (format)
        {
            case Format::R8:
            case Format::Depth16:
                return 1;
            case Format::BGRA8:
            case Format::RGBA8:
                return 4;
            default:
                SPARK_ASSERT(false, "GetNumTextureChannels - Unhandled Format");
        }

        return 0;
    }
}
