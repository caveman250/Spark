#include "DDS.h"

namespace se::asset::importer
{
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

    DDSData ReadDDSData(const builder::CompressedImageData &rawDDSData)
    {
        constexpr size_t s_FileCodeSize = 4;
        constexpr size_t s_HeaderSize = 124;

        // check file code
        char* ddsData = reinterpret_cast<char*>(rawDDSData.data);
        if (strncmp(ddsData, "DDS ", s_FileCodeSize) != 0)
        {
            debug::Log::Error("ReadDDSData - Not a valid DDS file");
            return {};
        }

        DDSData ret;

        uint8_t* header = reinterpret_cast<uint8_t*>(ddsData) + s_FileCodeSize;
        uint32_t width = *reinterpret_cast<uint32_t*>(&(header[8]));
        uint32_t height = *reinterpret_cast<uint32_t*>(&(header[12]));
        // uint32_t linearSize = *reinterpret_cast<uint32_t*>(&(header[16]));
        ret.m_MipCount = *reinterpret_cast<uint32_t*>(&(header[24]));
        uint32_t fourCC = *reinterpret_cast<uint32_t*>(&(header[80]));

        ret.m_Width = width;
        ret.m_Height = height;

        uint8_t* imageData = reinterpret_cast<uint8_t*>(ddsData) + s_FileCodeSize + s_HeaderSize;

        switch(fourCC)
        {
            case FOURCC_DXT5:
                ret.m_Format = texture::Format::DXT5;
                break;
            default:
                debug::Log::Error("ReadDDSData - Unhandled Texture Format! {:p}", fourCC);
        }

        uint32_t blockSize = 16;
        uint32_t offset = 0;

        for (unsigned int level = 0;
            ((ret.m_MipCount == 0 && level == 0) || (level < ret.m_MipCount)) && (width || height);
            ++level)
        {
            unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
            ret.m_Mips.push_back({ imageData + offset, size, width, height });
            offset += size;
            width  /= 2;
            height /= 2;
        }

        return ret;
    }
}