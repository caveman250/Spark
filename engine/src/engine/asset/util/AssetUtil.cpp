#include "AssetUtil.h"

#include "engine/string/util/StringUtil.h"

namespace se::asset::util
{
    std::string GetAssetSourcePath(const std::string& assetPath)
    {
        std::string ret = assetPath;
        constexpr const char* engineAssets = "/engine_assets";
        constexpr const char* engineSourceAssets = "/source_engine_assets";
        constexpr const char* appAssets = "/assets";
        constexpr const char* appSourceAssets = "/source_assets";
        if (ret.starts_with(engineAssets))
        {
            ret.replace(0, string::util::ConstLength(engineAssets), engineSourceAssets);
        }
        else if (ret.starts_with(appAssets))
        {
            ret.replace(0, string::util::ConstLength(appAssets), appSourceAssets);
        }

        if (ret.ends_with(".sass"))
        {
            ret.replace(ret.size() - 5,  5, ".json");
        }

        return ret;
    }
}
