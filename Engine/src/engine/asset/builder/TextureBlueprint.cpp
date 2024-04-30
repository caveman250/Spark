#include "TextureBlueprint.h"
#include "engine/io/VFS.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "crnlib.h"

namespace se::asset::builder
{
    std::regex TextureBlueprint::GetFilePattern() const
    {
        return std::regex(".*.(png|jpeg|jpg|tga|bmp|psd)");
    }

    std::shared_ptr<binary::Database> TextureBlueprint::BuildAsset(const std::string& path) const
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
        auto db = ToBinaryAsset(imageData, compressedData);
        FreeImage(imageData);
        FreeCompressedImage(compressedData);

        return db;
    }

    TextureBlueprint::ImageData TextureBlueprint::LoadImage(const std::string& path) const
    {
        ImageData ret;
        size_t srcImageSize;
        ret.sourceData = reinterpret_cast<uint8_t*>(io::VFS::Get().ReadBinary(path, srcImageSize));
        ret.data = reinterpret_cast<uint32_t*>(stbi_load_from_memory(ret.sourceData, srcImageSize, &ret.x, &ret.y, &ret.numComponents, 4));
        return ret;
    }

    TextureBlueprint::CompressedImageData TextureBlueprint::Compress(const ImageData& imageData) const
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

    void TextureBlueprint::FreeImage(const ImageData& imageData) const
    {
        stbi_image_free(imageData.data);
        std::free(imageData.sourceData);
    }

    void TextureBlueprint::FreeCompressedImage(const CompressedImageData& imageData) const
    {
        crn_free_block(imageData.data);
    }

    std::shared_ptr<binary::Database> TextureBlueprint::ToBinaryAsset(const ImageData& imageData, const CompressedImageData& compData) const
    {
        auto db = binary::Database::Create(false);
        asset::binary::StructLayout structLayout =
        {
            { asset::binary::CreateFixedString32("size"), asset::binary::Type::Vec2 },
            { asset::binary::CreateFixedString32("data"), asset::binary::Type::Blob },
        };
        auto structIndex1 = db->CreateStruct(structLayout);
        db->SetRootStruct(structIndex1);

        auto root = db->GetRoot();
        root.Set<math::Vec2>("size", math::Vec2(imageData.x, imageData.y));

        auto blob = db->CreateBlob(static_cast<const char*>(compData.data), compData.outputFileSize);
        root.Set<binary::Blob>("data", blob);

        return db;
    }
}
