#include "TextureBlueprint.h"
#include "engine/io/VFS.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/asset/texture/Texture.h"
#include "engine/io/OutputFileStream.h"
#include "engine/reflect/Util.h"
#include "stb_image.h"

#if SPARK_PLATFORM_MAC
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#elif SPARK_PLATFORM_LINUX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "rdo_bc_encoder.h"

#if SPARK_PLATFORM_MAC
#pragma clang diagnostic pop
#elif SPARK_PLATFORM_LINUX
#pragma GCC diagnostic pop
#endif

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
        if (!compressedData.GetData())
        {
            debug::Log::Error("Failed to compress image! {}", path);
            return { };
        }

        auto texture = Texture::FromRawData(imageData.x, imageData.y, compressedData, texture::Format::BC7);
        auto db = reflect::SerialiseType<Texture>(texture.get());

        FreeImage(imageData);

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

    memory::BinaryBlob TextureBlueprint::Compress(const RawImageData& imageData)
    {
        memory::BinaryBlob ret;

        rdo_bc::rdo_bc_params rp;
        rp.m_rdo_max_threads = 6;
        rp.m_status_output = true;

        rdo_bc::rdo_bc_encoder encoder;
        utils::image_u8 image(imageData.x, imageData.y);
        memcpy(static_cast<void*>(image.get_pixels().data()), static_cast<void*>(imageData.data), imageData.x * imageData.y * sizeof(uint32_t));

        if (!encoder.init(image, rp))
        {
            fprintf(stderr, "rdo_bc_encoder::init() failed!\n");
        }

        if (!encoder.encode())
        {
            fprintf(stderr, "rdo_bc_encoder::encode() failed!\n");
        }

        void* compressedImageData = malloc(encoder.get_total_blocks_size_in_bytes());
        memcpy(compressedImageData, encoder.get_blocks(), encoder.get_total_blocks_size_in_bytes());
        ret.SetData(compressedImageData);
        ret.SetSize(encoder.get_total_blocks_size_in_bytes());

        // crn_comp_params params;
        // params.m_width = imageData.x;
        // params.m_height = imageData.y;
        // params.set_flag(cCRNCompFlagPerceptual, true); //use srgb
        // params.set_flag(cCRNCompFlagHierarchical, true); // adapative block sizes
        // params.m_file_type = cCRNFileTypeDDS;
        // params.m_format = cCRNFmtDXT5;
        // params.m_pImages[0][0] = imageData.data;
        // params.m_num_helper_threads = std::min(15u, std::thread::hardware_concurrency() - 1u); // available threads - 1
        //
        // ret.data = crn_compress(params, ret.outputFileSize, &ret.outputQualityLevel, &ret.outputBitrate);
        return ret;
    }

    void TextureBlueprint::FreeImage(const RawImageData& imageData)
    {
        stbi_image_free(imageData.data);
        std::free(imageData.sourceData);
    }
}
