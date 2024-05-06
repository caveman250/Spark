#pragma once

#include "spark.h"

namespace se::asset::texture
{
    struct Mipmap
    {
        void* m_Data;
        uint32_t m_DataSize;
        uint32_t sizeX;
        uint32_t sizeY;
    };
}