#pragma once

#include <engine/asset/binary/Database.h>

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
        static std::shared_ptr<binary::Database> ProcessAsset(const std::string& assetPath, meta::MetaData& meta);
    };
}
