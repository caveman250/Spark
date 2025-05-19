#pragma once

#include "engine/asset/binary/Database.h"
#include "BuiltAsset.h"

namespace se::asset::meta
{
    class MetaData;
}
namespace se::asset::builder
{
    class AssetBuilder
    {
    public:
        static bool IsRelevantFile(const std::string& assetPath);
        static bool IsOutOfDate(const std::string& assetPath, const meta::MetaData& meta, const std::string& outputPath);
        static std::vector<BuiltAsset> ProcessAsset(const std::string& assetPath, const std::string& outputPath, meta::MetaData& meta);
    };
}
