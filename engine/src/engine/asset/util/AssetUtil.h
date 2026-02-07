#pragma once

#include "spark.h"

namespace se::asset::util
{
    std::string GetDataAssetSourcePath(std::string assetPath);
    std::string GetAssetBuiltPath(std::string assetPath);
}