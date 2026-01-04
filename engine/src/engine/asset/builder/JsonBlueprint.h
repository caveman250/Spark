#pragma once

#include "Blueprint.h"
#include "spark.h"

namespace se::asset::builder
{
    class JsonBlueprint : public Blueprint
    {
        uint32_t GetLatestVersion() const override { return 0; }
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath, meta::MetaData& meta) const override;
    };
}