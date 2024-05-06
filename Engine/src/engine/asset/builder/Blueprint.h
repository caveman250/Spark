#pragma once

#include <engine/asset/binary/Database.h>
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
        virtual std::shared_ptr<binary::Database> BuildAsset(const std::string& path, meta::MetaData& meta) const = 0;
    };
}
