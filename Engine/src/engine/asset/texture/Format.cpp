#include "Format.h"

DEFINE_SPARK_ENUM_BEGIN(se::asset::texture::Format)
    DEFINE_ENUM_VALUE(se::asset::texture::Format, R8)
    DEFINE_ENUM_VALUE(se::asset::texture::Format, DXT5)
    DEFINE_ENUM_VALUE(se::asset::texture::Format, BC7)
DEFINE_SPARK_ENUM_END()

namespace se::asset::texture
{
    bool IsCompressedFormat(Format::Type format)
    {
        switch (format)
        {
            case Format::R8:
                return false;
            case Format::DXT5:
            case Format::BC7:
                return true;
        }

        return false;
    }

    size_t GetCompressedFormatBlockSize(Format::Type format)
    {
        switch (format)
        {
            case Format::DXT5:
                return 4;
            case Format::BC7:
                return 4;
            default:
                SPARK_ASSERT(false, "GetCompressedFormatBlockSize - Unhandled Format");
                break;
        }

        return 0;
    }

    size_t GetCompressedFormatBlockSizeBytes(Format::Type format)
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

    size_t GetNumTextureChannels(Format::Type format)
    {
        switch (format)
        {
            case Format::R8:
                return 1;
            default:
                SPARK_ASSERT(false, "GetNumTextureChannels - Unhandled Format");
        }

        return 0;
    }
}
