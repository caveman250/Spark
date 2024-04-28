#pragma once

#include <engine/asset/binary/Database.h>
#include "spark.h"

namespace se::asset::builder
{
    class Blueprint
    {
    public:
        virtual ~Blueprint() {}
        virtual std::regex GetFilePattern() const = 0;
        virtual std::shared_ptr<binary::Database> BuildAsset(const std::string& path) const = 0;
    };
}
