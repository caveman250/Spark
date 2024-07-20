#pragma once

#include "spark.h"
#include "engine/memory/BinaryBlob.h"

namespace se::asset::texture
{
    struct Mipmap
    {
        DECLARE_SPARK_CLASS(Mipmap)

        memory::BinaryBlob m_Data;
        uint32_t sizeX;
        uint32_t sizeY;
    };
}
