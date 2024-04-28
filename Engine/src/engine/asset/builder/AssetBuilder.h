#pragma once

#include <engine/asset/binary/Database.h>

namespace se::asset::builder
{
    class AssetBuilder
    {
    public:
        static std::shared_ptr<binary::Database> ProcessAsset(const std::string& assetPath);
    };
}
