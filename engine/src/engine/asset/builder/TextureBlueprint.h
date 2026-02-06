#pragma once

#include <engine/memory/BinaryBlob.h>
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

    class TextureBlueprint : public Blueprint
    {
    public:
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const override;

        static RawImageData LoadImage(const std::string& path);
        static RawImageData LoadImageFromBytes(void* bytes, size_t size);
        static memory::BinaryBlob Compress(const RawImageData& imageData);
        static void FreeImage(const RawImageData& imageData);

    private:
        uint32_t GetLatestVersion() const { return 2; }
        bool IsOutOfDate(const std::string& assetPath, const std::string& outputPath) override;
    };
}
