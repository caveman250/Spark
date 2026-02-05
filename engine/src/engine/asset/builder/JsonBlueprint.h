#pragma once

#include "Blueprint.h"
#include "spark.h"

namespace se::asset::builder
{
    class JsonBlueprint : public Blueprint
    {
        std::regex GetFilePattern() const override;
        std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const override;
    };
}