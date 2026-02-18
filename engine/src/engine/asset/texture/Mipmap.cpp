#include "Mipmap.h"

namespace se::asset::texture
{
    Mipmap::Mipmap(const memory::BinaryBlob& data,
                     const uint32_t sizeX,
                     const uint32_t sizeY)
        : m_Data(data)
        , m_SizeX(sizeX)
        , m_SizeY(sizeY)
    {}
}