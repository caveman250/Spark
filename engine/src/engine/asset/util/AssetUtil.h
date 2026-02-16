#pragma once

#include "spark.h"

namespace se::asset::util
{
    std::string GetSourcePath(std::string assetPath, const std::string& fileExt);
    std::string GetAssetBuiltPath(std::string assetPath);
}