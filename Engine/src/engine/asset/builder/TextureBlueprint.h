#pragma once

#include "Blueprint.h"

namespace se::asset::builder
{
    class TextureBlueprint : public Blueprint
    {
    public:
        std::regex GetFilePattern() const override;
        std::shared_ptr<binary::Database> BuildAsset(const std::string& path) const override;
    };
}
