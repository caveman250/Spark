module;

#include <memory>

export module Spark.Render.Util.TextureUtil;
import Spark.Render.TextureResource;
import Spark.Asset.Texture;

namespace se::render::util
{
    export class TextureUtil
    {
    public:
        static std::shared_ptr<TextureResource> CreateTextureResource(const asset::Texture& texture);
    };
}
