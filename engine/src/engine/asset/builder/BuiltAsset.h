#pragma once

#include "engine/asset/binary/Database.h"

namespace se::asset::builder
{
    struct BuiltAsset
    {
        std::shared_ptr<binary::Database> db;
        std::string fileNameSuffix;
    };
}
