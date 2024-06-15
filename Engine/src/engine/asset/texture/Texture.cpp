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

    Texture::~Texture()
    {
        for (uint32_t i = 0; i < m_MipCount; ++i)
        {
            if (m_Mips[i].m_Data)
            {
                std::free(m_Mips[i].m_Data);
            }
        }
    }

    std::shared_ptr<binary::Database> Texture:: Serialise()
    {
        auto db = binary::Database::Create(false);
        binary::StructLayout rootStructLayout =
        {
            {binary::CreateFixedString32("sizeX"), binary::Type::Uint32},
            {binary::CreateFixedString32("sizeY"), binary::Type::Uint32},
            {binary::CreateFixedString32("mipCount"), binary::Type::Uint32},
            {binary::CreateFixedString32("format"), binary::Type::Uint32},
            {binary::CreateFixedString32("mips"), binary::Type::Array}
        };

        auto rootStructIndex = db->CreateStruct(rootStructLayout);
        db->SetRootStruct(rootStructIndex);

        auto root = db->GetRoot();
        root.Set<uint32_t>("sizeX", m_Width);
        root.Set<uint32_t>("sizeX", m_Height);
        root.Set<uint32_t>("mipCount", m_MipCount);
        root.Set<uint32_t>("format", static_cast<uint32_t>(m_Format));

        binary::StructLayout mipStructLayout =
        {
            {binary::CreateFixedString32("data"), binary::Type::Blob},
            {binary::CreateFixedString32("sizeX"), binary::Type::Uint32},
            {binary::CreateFixedString32("sizeY"), binary::Type::Uint32}
        };

        auto mipStructIndex = db->CreateStruct(mipStructLayout);
        auto mipArray = db->CreateArray(mipStructIndex, m_MipCount);
        root.Set<binary::Array>("mips", mipArray);
        for (uint32_t i = 0; i < m_MipCount; ++i)
        {
            const auto& mip = m_Mips[i];
            auto mipObj = mipArray.Get(i);
            mipObj.Set("sizeX", mip.sizeX);
            mipObj.Set("sizeY", mip.sizeY);

            auto blob = db->CreateBlob(static_cast<const char*>(mip.m_Data), mip.m_DataSize);
            mipObj.Set<binary::Blob>("data", blob);
        }

        return db;
    }

    void Texture::Deserialise(const std::shared_ptr<binary::Database>& db)
    {
        auto root = db->GetRoot();
        m_Width = root.Get<uint32_t>("sizeX");
        m_Height = root.Get<uint32_t>("sizeX");
        m_MipCount = root.Get<uint32_t>("mipCount");
        m_Format = static_cast<texture::Format::Type>(root.Get<uint32_t>("format"));

        auto mipArray = root.Get<binary::Array>("mips");
        for (uint32_t i = 0; i < m_MipCount; ++i)
        {
            auto& mip = m_Mips.emplace_back();
            auto mipObj = mipArray.Get(i);
            mip.sizeX = mipObj.Get<uint32_t>("sizeX");
            mip.sizeY = mipObj.Get<uint32_t>("sizeY");

            auto blob = mipObj.Get<binary::Blob>("data");
            mip.m_Data = std::malloc(blob.GetSize());
            std::memcpy(mip.m_Data, blob.GetData(), blob.GetSize());
            mip.m_DataSize = blob.GetSize();
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
            ret->m_Mips.push_back({mipData, size, width, height});
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
