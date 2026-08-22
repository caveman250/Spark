module;

#include "spark.h"

export module Spark.Asset.Meta.TextureMetaData;
export import Spark.Asset.Meta.MetaData;
import Spark.Asset.Texture.Format;

namespace se::asset::meta 
{
    export class TextureMetaData : public MetaData
    {
        SPARK_CLASS()

        TextureMetaData(const std::string& assetPath) : MetaData(assetPath) {};

        SPARK_MEMBER(Serialized)
        texture::Format m_Format = texture::Format::BC7;

        SPARK_MEMBER(Serialized)
        texture::Usage m_Usage = texture::Usage::Read;
    };
}