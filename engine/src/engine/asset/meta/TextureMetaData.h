#pragma once 

#include "spark.h"
#include "MetaData.h"
#include "engine/asset/texture/Format.h"

namespace se::asset::meta 
{
    class TextureMetaData : public MetaData
    {
        SPARK_CLASS()

        TextureMetaData(const std::string& assetPath) : MetaData(assetPath) {};

        SPARK_MEMBER(Serialized)
        texture::Format m_Format = texture::Format::BC7;

        SPARK_MEMBER(Serialized)
        texture::Usage m_Usage = texture::Usage::Read;
    };
}