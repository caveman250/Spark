#pragma once

#include "spark.h"
#include "Blueprint.h"

namespace se::asset::builder
{
    class ShaderBlueprint : public Blueprint
    {
    public:
        uint32_t GetLatestVersion() const override { return 0; }
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath, meta::MetaData& meta) const override;
    };
}
