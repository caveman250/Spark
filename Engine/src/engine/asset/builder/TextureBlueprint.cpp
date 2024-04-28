#include "TextureBlueprint.h"
#include "engine/io/FileSystem.h"
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
        // load the source image
        size_t srcImageSize;
        uint8_t* sourceData = reinterpret_cast<uint8_t*>(io::FileSystem::ReadBinaryFile(path, srcImageSize));
        int x, y, componentCount;
        stbi_uc* sourceImage = stbi_load_from_memory(sourceData, srcImageSize, &x, &y, &componentCount, 4);

        crn_comp_params params;
        params.m_width = x;
        params.m_height = y;
        params.set_flag(cCRNCompFlagPerceptual, true); //use srgb
        params.set_flag(cCRNCompFlagHierarchical, true); // adapative block sizes
        params.m_file_type = cCRNFileTypeDDS;
        params.m_format = cCRNFmtDXT5;
        params.m_pImages[0][0] = reinterpret_cast<const crn_uint32*>(sourceImage);
        params.m_num_helper_threads = std::thread::hardware_concurrency() - 1; // available threads - 1

        crn_uint32 outputQualityLevel;
        float outputBitrate;
        crn_uint32 outputFileSize;
        void* dduData = crn_compress(params, outputFileSize, &outputQualityLevel, &outputBitrate);

        stbi_image_free(sourceImage);
        std::free(sourceData);

        return std::shared_ptr<binary::Database>();
    }
}