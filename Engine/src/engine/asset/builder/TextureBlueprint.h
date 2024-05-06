#pragma once

#include "Blueprint.h"

namespace se::asset::builder
{
    struct RawImageData
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

    class TextureBlueprint : public Blueprint
    {
    public:
        std::regex GetFilePattern() const override;
        std::shared_ptr<binary::Database> BuildAsset(const std::string& path) const override;

    private:
        RawImageData LoadImage(const std::string& path) const;
        CompressedImageData Compress(const RawImageData& imageData) const;
        void FreeImage(const RawImageData& imageData) const;
        void FreeCompressedImage(const CompressedImageData& imageData) const;
        std::shared_ptr<binary::Database> ToBinaryAsset(const CompressedImageData& compData) const;

    };
}
