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
        virtual uint32_t GetLatestVersion() const = 0;
        virtual std::regex GetFilePattern() const = 0;
        virtual std::vector<BuiltAsset> BuildAsset(const std::string& path, const std::string& outputPath, meta::MetaData& meta) const = 0;
    };
}
