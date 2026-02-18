#include "TextureBlueprint.h"
#include "engine/io/VFS.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/asset/texture/Texture.h"
#include "engine/reflect/Util.h"
#include "stb_image.h"
#include "engine/asset/meta/MetaData.h"
#include "engine/asset/meta/MetaDataManager.h"
#include "engine/asset/meta/TextureMetaData.h"

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

    std::vector<BuiltAsset> TextureBlueprint::BuildAsset(const std::string& path, const std::string&) const
    {
        const auto metaManager = meta::MetaManager::Get();
        const auto metaData = metaManager->GetOrCreateMetaDataForAsset<meta::TextureMetaData>(path);

        const auto imageData = LoadImage(path);
        if (!imageData.data)
        {
            debug::Log::Error("Failed to load image {}", path);
            return { };
        }

        SPARK_ASSERT(metaData->m_Format == texture::Format::BC7, "Non BC7 texture formats not supported currently.");
        const auto compressedData = Compress(imageData);
        if (compressedData.GetData() == nullptr)
        {
            debug::Log::Error("Failed to compress image! {}", path);
            return { };
        }

        const auto texture = Texture::FromRawData(imageData.x, imageData.y, compressedData, metaData->m_Format, metaData->m_Usage);
        texture->m_SourcePath = path;
        const auto db = reflect::SerialiseType<Texture>(texture.get());

        FreeImage(imageData);

        metaData->SetFormatVersion(GetLatestVersion());
        metaManager->SaveMetaData(metaData);

        return { { db, "" } };
    }

    RawImageData TextureBlueprint::LoadImage(const std::string& path)
    {
        size_t srcImageSize;
        void* data = io::VFS::Get().ReadBinary(path, srcImageSize);
        return LoadImageFromBytes(data, srcImageSize);
    }

    RawImageData TextureBlueprint::LoadImageFromBytes(void *bytes, const size_t size)
    {
        RawImageData ret = {};
        ret.sourceData = static_cast<uint8_t*>(bytes);
        ret.data = reinterpret_cast<uint32_t*>(stbi_load_from_memory(ret.sourceData, static_cast<int>(size), &ret.x, &ret.y, &ret.numComponents, 4));
        return ret;
    }

    memory::BinaryBlob TextureBlueprint::Compress(const RawImageData& imageData)
    {
        memory::BinaryBlob ret;

        rdo_bc::rdo_bc_params rp = {};
        rp.m_rdo_max_threads = 6;
        // uncomment to see b7enc logs.
        //rp.m_status_output = true;

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

        return ret;
    }

    void TextureBlueprint::FreeImage(const RawImageData& imageData)
    {
        stbi_image_free(imageData.data);
        std::free(imageData.sourceData);
    }

    bool TextureBlueprint::IsOutOfDate(const std::string& assetPath, const std::string& outputPath)
    {
        const auto metaManager = meta::MetaManager::Get();
        const auto metaData = metaManager->GetOrCreateMetaDataForAsset<meta::TextureMetaData>(assetPath);
        if (metaData->GetFormatVersion() < GetLatestVersion())
        {
            return true;
        }

        return Blueprint::IsOutOfDate(assetPath, outputPath);
    }
}
