#include "TextureBlueprint.h"
#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "crnlib.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/asset/texture/Texture.h"
#include "engine/io/OutputFileStream.h"
#include "engine/reflect/Util.h"
#include "stb_image.h"

namespace se::asset::builder
{
    std::regex TextureBlueprint::GetFilePattern() const
    {
        return std::regex(".*.(png|jpeg|jpg|tga|bmp|psd)");
    }

    std::vector<BuiltAsset> TextureBlueprint::BuildAsset(const std::string& path, const std::string&, meta::MetaData&) const
    {
        auto imageData = LoadImage(path);
        if (!imageData.data)
        {
            debug::Log::Error("Failed to load image {}", path);
            return { };
        }
        auto compressedData = Compress(imageData);
        if (!compressedData.data)
        {
            debug::Log::Error("Failed to compress image! {}", path);
            return { };
        }

        auto texture = Texture::FromDDS(compressedData);
        auto db = reflect::SerialiseType<Texture>(texture.get());

        FreeImage(imageData);
        FreeCompressedImage(compressedData);

        return { { db, "" } };
    }

    RawImageData TextureBlueprint::LoadImage(const std::string& path)
    {
        size_t srcImageSize;
        void* data = io::VFS::Get().ReadBinary(path, srcImageSize);
        return LoadImageFromBytes(data, srcImageSize);
    }

    RawImageData TextureBlueprint::LoadImageFromBytes(void *bytes, size_t size)
    {
        RawImageData ret = {};
        ret.sourceData = reinterpret_cast<uint8_t*>(bytes);
        ret.data = reinterpret_cast<uint32_t*>(stbi_load_from_memory(ret.sourceData, static_cast<int>(size), &ret.x, &ret.y, &ret.numComponents, 4));
        return ret;
    }

    CompressedImageData TextureBlueprint::Compress(const RawImageData& imageData)
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

    void TextureBlueprint::FreeImage(const RawImageData& imageData)
    {
        stbi_image_free(imageData.data);
        std::free(imageData.sourceData);
    }

    void TextureBlueprint::FreeCompressedImage(const CompressedImageData& imageData)
    {
        crn_free_block(imageData.data);
    }
}
