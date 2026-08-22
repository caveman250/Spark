module;

#include <memory>

module Spark.Render.Util.TextureUtil;
import Spark.Render.TextureResource;
import Spark.Render.Metal.TextureResource;

namespace se::render::util
{
    std::shared_ptr<TextureResource> TextureUtil::CreateTextureResource(const asset::Texture& texture)
    {
        return std::make_shared<metal::TextureResource>(texture.GetWidth(), texture.GetHeight(),
            texture.GetFormat(), texture.GetUsage(), texture.GetMips());
    }
}
