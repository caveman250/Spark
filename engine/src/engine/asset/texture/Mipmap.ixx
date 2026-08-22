module;

#include "spark.h"
#include "engine/reflect/Object.h"

export module Spark.Asset.Texture.Mipmap;
import Spark.Memory.BinaryBlob;

namespace se::asset::texture
{
    export struct Mipmap : reflect::ObjectBase
    {
        SPARK_CLASS()
        Mipmap() = default;
        Mipmap(const memory::BinaryBlob& data, uint32_t sizeX, uint32_t sizeY);

        SPARK_MEMBER(Serialized)
        memory::BinaryBlob m_Data = {};

        SPARK_MEMBER(Serialized)
        uint32_t m_SizeX = 0;

        SPARK_MEMBER(Serialized)
        uint32_t m_SizeY = 0;
    };
}
