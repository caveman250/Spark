#include <engine/asset/importer/DDS.h>
#include "TextureBlueprint.h"
#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "crnlib.h"

namespace se::asset::builder
{
    std::regex TextureBlueprint::GetFilePattern() const
    {
        return std::regex(".*.(png|jpeg|jpg|tga|bmp|psd)");
    }

    std::shared_ptr<binary::Database> TextureBlueprint::BuildAsset(const std::string& path, meta::MetaData& meta) const
    {
        auto imageData = LoadImage(path);
        if (!imageData.data)
        {
            debug::Log::Error("Failed to load image {}", path);
            return nullptr;
        }
        auto compressedData = Compress(imageData);
        if (!compressedData.data)
        {
            debug::Log::Error("Failed to compress image! {}", path);
            return nullptr;
        }
        auto db = ToBinaryAsset(compressedData);
        FreeImage(imageData);
        FreeCompressedImage(compressedData);

        meta.SetFormatVersion(GetLatestVersion());

        return db;
    }

    RawImageData TextureBlueprint::LoadImage(const std::string& path) const
    {
        RawImageData ret;
        size_t srcImageSize;
        ret.sourceData = reinterpret_cast<uint8_t*>(io::VFS::Get().ReadBinary(path, srcImageSize));
        ret.data = reinterpret_cast<uint32_t*>(stbi_load_from_memory(ret.sourceData, static_cast<int>(srcImageSize), &ret.x, &ret.y, &ret.numComponents, 4));
        return ret;
    }

    CompressedImageData TextureBlueprint::Compress(const RawImageData& imageData) const
    {
        CompressedImageData ret;

        crn_comp_params params;
        params.m_width = imageData.x;
        params.m_height = imageData.y;
        params.set_flag(cCRNCompFlagPerceptual, true); //use srgb
        params.set_flag(cCRNCompFlagHierarchical, true); // adapative block sizes
        params.m_file_type = cCRNFileTypeDDS;
        params.m_format = cCRNFmtDXT5;
        params.m_pImages[0][0] = imageData.data;
        params.m_num_helper_threads = std::min(15u, std::thread::hardware_concurrency() - 1u); // available threads - 1

        ret.data = crn_compress(params, ret.outputFileSize, &ret.outputQualityLevel, &ret.outputBitrate);
        return ret;
    }

    void TextureBlueprint::FreeImage(const RawImageData& imageData) const
    {
        stbi_image_free(imageData.data);
        std::free(imageData.sourceData);
    }

    void TextureBlueprint::FreeCompressedImage(const CompressedImageData& imageData) const
    {
        crn_free_block(imageData.data);
    }

    std::shared_ptr<binary::Database> TextureBlueprint::ToBinaryAsset(const CompressedImageData& compData) const
    {
        importer::DDSData ddsData = importer::ReadDDSData(compData);

        auto db = binary::Database::Create(false);
        asset::binary::StructLayout rootStructLayout =
        {
            { asset::binary::CreateFixedString32("sizeX"), asset::binary::Type::Uint32 },
            { asset::binary::CreateFixedString32("sizeY"), asset::binary::Type::Uint32 },
            { asset::binary::CreateFixedString32("mipCount"), asset::binary::Type::Uint32 },
            { asset::binary::CreateFixedString32("format"), asset::binary::Type::Uint32 },
            { asset::binary::CreateFixedString32("mips"), asset::binary::Type::Array }
        };

        auto rootStructIndex = db->CreateStruct(rootStructLayout);
        db->SetRootStruct(rootStructIndex);

        auto root = db->GetRoot();
        root.Set<uint32_t>("sizeX", ddsData.m_Width);
        root.Set<uint32_t>("sizeX", ddsData.m_Height);
        root.Set<uint32_t>("mipCount", ddsData.m_MipCount);
        root.Set<uint32_t>("format", static_cast<uint32_t>(ddsData.m_Format));

        asset::binary::StructLayout mipStructLayout =
        {
            { asset::binary::CreateFixedString32("data"), asset::binary::Type::Blob },
            { asset::binary::CreateFixedString32("sizeX"), asset::binary::Type::Uint32 },
            { asset::binary::CreateFixedString32("sizeY"), asset::binary::Type::Uint32 }
        };

        auto mipStructIndex = db->CreateStruct(mipStructLayout);
        auto mipArray = db->CreateArray(mipStructIndex, ddsData.m_MipCount);
        for (uint32_t i = 0; i < ddsData.m_MipCount; ++i)
        {
            const auto& mip = ddsData.m_Mips[i];
            auto mipObj = mipArray.Get(i);
            mipObj.Set("sizeX", mip.sizeX);
            mipObj.Set("sizeY", mip.sizeY);

            auto blob = db->CreateBlob(static_cast<const char*>(mip.m_Data), mip.m_DataSize);
            mipObj.Set<binary::Blob>("data", blob);
        }

        return db;
    }
}
