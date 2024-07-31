#include "Texture.h"

#include "Format.h"
#include "Mipmap.h"
#include "engine/asset/binary/binary.h"
#include "engine/asset/binary/Database.h"
#include "engine/asset/builder/TextureBlueprint.h"
#include "engine/render/TextureResource.h"

namespace se::asset
{
DEFINE_SPARK_CLASS_BEGIN(Texture)
    DEFINE_MEMBER(m_Width)
    DEFINE_MEMBER(m_Height)
    DEFINE_MEMBER(m_MipCount)
    DEFINE_MEMBER(m_Mips)
    DEFINE_MEMBER(m_Format)
DEFINE_SPARK_CLASS_END()


    void Texture::Release()
    {
        for (auto& mip : m_Mips)
        {
            mip.m_Data.Release();
        }
    }

    std::shared_ptr<Texture>  Texture::FromDDS(const builder::CompressedImageData& rawDDSData)
    {
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

        constexpr size_t s_FileCodeSize = 4;
        constexpr size_t s_HeaderSize = 124;

        // check file code
        char* ddsData = reinterpret_cast<char*>(rawDDSData.data);
        if (strncmp(ddsData, "DDS ", s_FileCodeSize) != 0)
        {
            debug::Log::Error("ReadDDSData - Not a valid DDS file");
            return {};
        }

        std::shared_ptr<Texture> ret = std::make_shared<Texture>();

        uint8_t* header = reinterpret_cast<uint8_t*>(ddsData) + s_FileCodeSize;
        uint32_t width = *reinterpret_cast<uint32_t*>(&(header[8]));
        uint32_t height = *reinterpret_cast<uint32_t*>(&(header[12]));
        // uint32_t linearSize = *reinterpret_cast<uint32_t*>(&(header[16]));
        ret->m_MipCount = *reinterpret_cast<uint32_t*>(&(header[24]));
        uint32_t fourCC = *reinterpret_cast<uint32_t*>(&(header[80]));

        if (ret->m_MipCount == 0)
        {
            ret->m_MipCount++;
        }

        ret->m_Width = width;
        ret->m_Height = height;

        uint8_t* imageData = reinterpret_cast<uint8_t*>(ddsData) + s_FileCodeSize + s_HeaderSize;

        switch (fourCC)
        {
            case FOURCC_DXT5:
                ret->m_Format = texture::Format::DXT5;
                break;
            default:
                debug::Log::Error("ReadDDSData - Unhandled Texture Format! {:p}", fourCC);
        }

        uint32_t blockSize = 16;
        uint32_t offset = 0;

        for (unsigned int level = 0;
             ((ret->m_MipCount == 0 && level == 0) || (level < ret->m_MipCount)) && (width || height);
             ++level)
        {
            unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;

            void* mipData = std::malloc(size);
            std::memcpy(mipData, imageData + offset, size);
            ret->m_Mips.push_back(texture::Mipmap(memory::BinaryBlob(mipData, size), width, height));
            offset += size;
            width /= 2;
            height /= 2;
        }

        return ret;
    }

    void Texture::CreatePlatformResource()
    {
        m_PlatformResource = render::TextureResource::Create(*this);
        m_PlatformResource->CreatePlatformResources();
    }

    const std::shared_ptr<render::TextureResource> &Texture::GetPlatformResource() const
    {
        return m_PlatformResource;
    }
}
