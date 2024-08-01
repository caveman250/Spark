#pragma once

#include "spark.h"
#include "Blueprint.h"

namespace se::asset::builder
{
    class ShaderBlueprint : public Blueprint
    {
    public:
        uint32_t GetLatestVersion() const override { return 0; }
        std::regex GetFilePattern() const override;
        std::shared_ptr<binary::Database> BuildAsset(const std::string& path, meta::MetaData& meta) const override;
    };
}
