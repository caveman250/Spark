#pragma once

#include "spark.h"
#include "Blueprint.h"

namespace se::asset::builder
{
    class FBXBlueprint : public Blueprint
    {
    public:
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const override;

    private:
        uint32_t GetLatestVersion() const { return 2; }
        bool IsOutOfDate(const std::string& assetPath, const std::string& outputPath) override;
    };
}
