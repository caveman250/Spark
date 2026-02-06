#pragma once

#include "BuiltAsset.h"
#include "spark.h"

namespace se::asset::meta
{
    class MetaData;
}
namespace se::asset::builder
{
    class Blueprint
    {
    public:
        virtual ~Blueprint() {}
        virtual std::regex GetFilePattern() const = 0;
        virtual std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath) const = 0;
        virtual bool IsOutOfDate([[maybe_unused]] const std::string& assetPath, [[maybe_unused]] const std::string& outputPath);
    };
}
