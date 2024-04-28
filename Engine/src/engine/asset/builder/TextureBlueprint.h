#pragma once

#include "Blueprint.h"

namespace se::asset::builder
{
    class TextureBlueprint : public Blueprint
    {
    public:
        std::regex GetFilePattern() const override;
        std::shared_ptr<binary::Database> BuildAsset(const std::string& path) const override;

    private:
        struct ImageData
        {
            uint32_t* data;
            uint8_t* sourceData;
            int x;
            int y;
            int numComponents;
        };
        struct CompressedImageData
        {
            void* data;
            uint32_t outputQualityLevel;
            float outputBitrate;
            uint32_t outputFileSize;
        };
        ImageData LoadImage(const std::string& path) const;
        CompressedImageData Compress(const ImageData& imageData) const;
        void FreeImage(const ImageData& imageData) const;
        void FreeCompressedImage(const CompressedImageData& imageData) const;
        std::shared_ptr<binary::Database> ToBinaryAsset(const ImageData& imageData, const CompressedImageData& compData) const;
    };
}
