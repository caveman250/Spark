#include "AssetUtil.h"

#include "engine/asset/Asset.h"
#include "engine/string/util/StringUtil.h"

namespace se::asset::util
{
    std::string GetDataAssetSourcePath(std::string assetPath)
    {
        constexpr const char* engineAssets = "/engine_assets";
        constexpr const char* engineSourceAssets = "/source_engine_assets";
        constexpr const char* appAssets = "/assets";
        constexpr const char* appSourceAssets = "/source_assets";
        if (assetPath.starts_with(engineAssets))
        {
            assetPath.replace(0, string::util::ConstLength(engineAssets), engineSourceAssets);
        }
        else if (assetPath.starts_with(appAssets))
        {
            assetPath.replace(0, string::util::ConstLength(appAssets), appSourceAssets);
        }

        if (assetPath.ends_with(".sass"))
        {
            assetPath.replace(assetPath.size() - 5,  5, ".json");
        }

        return assetPath;
    }
}
