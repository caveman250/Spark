#pragma once

#include "spark.h"
#include "engine/memory/BinaryBlob.h"

namespace se::asset::texture
{
    struct Mipmap : reflect::ObjectBase
    {
        DECLARE_SPARK_CLASS(Mipmap)
        Mipmap();
        Mipmap(memory::BinaryBlob m_Data, uint32_t sizeX, uint32_t sizeY);

        memory::BinaryBlob m_Data;
        uint32_t m_SizeX;
        uint32_t m_SizeY;
    };
}
