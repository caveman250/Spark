#pragma once

#include "engine/asset/binary/Database.h"
#include "BuiltAsset.h"
#include "Blueprint.h"

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
        static bool IsOutOfDate(const std::string& assetPath, Blueprint* bp, const std::string& outputPath);
        static Blueprint* GetBlueprintForAsset(const std::string& assetPath);
    };
}
